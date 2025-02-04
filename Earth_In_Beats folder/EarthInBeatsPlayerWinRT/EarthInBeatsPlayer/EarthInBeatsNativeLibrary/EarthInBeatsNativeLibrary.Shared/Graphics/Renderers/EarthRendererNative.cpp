#include "EarthRendererNative.h"
#include "..\Helpers\H.h"
#include "DxRenderingContext.h"
#include "..\Helpers\ImageUtils.h"
#include "Vertex.h"

#include <fstream>
#include <cassert>
#include <cstdint>
#include <emmintrin.h>

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

const float EarthRendererNative::HORIZONTAL_ROTATION_FACTOR = 35.0f;

EarthRendererNative::EarthRendererNative() :
	initialized(false), modelLoaded(false), indexCount(0),
	earthRotationEnabled(false), scale(1.3f),
	rotationAngleHorizontal(0.0f), rotationAngleVertical(0.0f), isSongPlayingNow(false),
	managingByEarthManipulations(false), angleForRewinding(0.0f), angleForVolumeChange(0.0f)
{
	DirectX::XMStoreFloat4x4(&this->projection, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&this->matrixRotation, DirectX::XMMatrixRotationRollPitchYaw(0.0f, DirectX::XMConvertToRadians(250.0f), 0.0f));

	this->tapTimer = std::make_unique<Timer>();
	this->tapCount = 0;
	this->HORIZONTAL_ANGLE_LIMITER_TOP = 0.0f;
	this->HORIZONTAL_ANGLE_LIMITER_BOTTOM = 0.0f;
	this->songDuration = 0.0;
}

EarthRendererNative::~EarthRendererNative() {
}

void EarthRendererNative::Initialize(const std::shared_ptr<GuardedDeviceResources> &dx) {
	this->initializeTask = concurrency::create_task([=]() {
		this->dx = dx;
		auto dxDev = this->dx->Get();

		concurrency::critical_section::scoped_lock lk(this->dataCs);

		HRESULT hr = S_OK;
		auto d3dDev = dxDev->GetD3DDevice();
		auto d3dCtx = dxDev->GetD3DDeviceContext();

		auto size = dxDev->GetLogicalSize();

		// load Shaders from shader files
		// for 3d model
		auto vertexShaderData = HSystem::LoadPackageFile(L"EarthInBeatsNativeLibrary\\VertexShader.cso");
		auto pixelShaderData = HSystem::LoadPackageFile(L"EarthInBeatsNativeLibrary\\PixelShader.cso");

		// for 2d background
		auto squareVertexShaderData = HSystem::LoadPackageFile(L"EarthInBeatsNativeLibrary\\SquareVertexShader.cso");
		auto squarePixelShaderData = HSystem::LoadPackageFile(L"EarthInBeatsNativeLibrary\\SquarePixelShader.cso");

		// for 3d model
		hr = d3dDev->CreateVertexShader(vertexShaderData.data(), vertexShaderData.size(), NULL, this->vertexShader.GetAddressOf());
		HSystem::ThrowIfFailed(hr);

		hr = d3dDev->CreatePixelShader(pixelShaderData.data(), pixelShaderData.size(), NULL, this->pixelShader.GetAddressOf());
		HSystem::ThrowIfFailed(hr);

		// for 2d background
		hr = d3dDev->CreateVertexShader(squareVertexShaderData.data(), squareVertexShaderData.size(), NULL, this->squareVertexShader.GetAddressOf());
		HSystem::ThrowIfFailed(hr);

		hr = d3dDev->CreatePixelShader(squarePixelShaderData.data(), squarePixelShaderData.size(), NULL, this->squarePixelShader.GetAddressOf());
		HSystem::ThrowIfFailed(hr);

		// Describe the Sample State
		D3D11_SAMPLER_DESC sampDesc;
		ZeroMemory(&sampDesc, sizeof(sampDesc));
		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

		//Create the Sample State
		hr = d3dDev->CreateSamplerState(&sampDesc, this->sampler.GetAddressOf());
		HSystem::ThrowIfFailed(hr);

		// Define the input layout
		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 2, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		D3D11_INPUT_ELEMENT_DESC layoutBg[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		auto numElements = sizeof(layout) / sizeof(layout[0]);
		auto numElements2 = sizeof(layoutBg) / sizeof(layoutBg[0]);

		// 3d model
		hr = d3dDev->CreateInputLayout(layout, numElements, vertexShaderData.data(), vertexShaderData.size(), this->inputLayout.GetAddressOf());
		HSystem::ThrowIfFailed(hr);

		// 2d background
		hr = d3dDev->CreateInputLayout(layoutBg, numElements2, squareVertexShaderData.data(), squareVertexShaderData.size(), this->inputLayoutBackground.GetAddressOf());
		HSystem::ThrowIfFailed(hr);

		d3dCtx->IASetInputLayout(this->inputLayout.Get());
		d3dCtx->IASetInputLayout(this->inputLayoutBackground.Get());

		d3dCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// create constBuffer
		D3D11_BUFFER_DESC constBuff = { 0 };
		constBuff.Usage = D3D11_USAGE_DEFAULT;
		constBuff.ByteWidth = sizeof(ConstantBufferData);
		constBuff.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constBuff.CPUAccessFlags = 0;
		constBuff.MiscFlags = 0;

		hr = d3dDev->CreateBuffer(&constBuff, nullptr, this->constantBuffer.GetAddressOf());
		HSystem::ThrowIfFailed(hr);

		D3D11_RASTERIZER_DESC rsDesc;
		rsDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
		rsDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_FRONT;
		rsDesc.FrontCounterClockwise = FALSE;
		rsDesc.DepthBias = 0;
		rsDesc.SlopeScaledDepthBias = 0.0f;
		rsDesc.DepthBiasClamp = 0.0f;
		rsDesc.DepthClipEnable = TRUE;
		rsDesc.ScissorEnable = FALSE;
		rsDesc.MultisampleEnable = FALSE;
		rsDesc.AntialiasedLineEnable = FALSE;

		hr = d3dDev->CreateRasterizerState(&rsDesc, this->rsState.GetAddressOf());
		H::System::ThrowIfFailed(hr);

		// create index buffer, vertex buffer and texture buffer for render background texture
		DirectX::XMFLOAT3 backgroundTexturePositions[] = {
			DirectX::XMFLOAT3(-1.0f * 2, -1.0f, -1.0f),
			DirectX::XMFLOAT3(-1.0f * 2, 1.0f, -1.0f),
			DirectX::XMFLOAT3(1.0f * 2, 1.0f, -1.0f),
			DirectX::XMFLOAT3(1.0f * 2, -1.0f, -1.0f)
		};

		DirectX::XMFLOAT2 backgroundTextureTexCoords[] = {
			DirectX::XMFLOAT2(0.0f, 1.0f),
			DirectX::XMFLOAT2(0.0f, 0.0f),
			DirectX::XMFLOAT2(1.0f, 0.0f),
			DirectX::XMFLOAT2(1.0f, 1.0f)
		};

		DWORD backgroundTextureIndices[] = {
			0,  1,  2,
			0,  2,  3
		};

		// create Index buffer for background
		D3D11_BUFFER_DESC indexBufferDesc = { 0 };
		indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		indexBufferDesc.ByteWidth = sizeof(DWORD) * 6;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;
		indexBufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA idxInitData;
		ZeroMemory(&idxInitData, sizeof(idxInitData));
		idxInitData.pSysMem = backgroundTextureIndices;

		hr = d3dDev->CreateBuffer(&indexBufferDesc, &idxInitData, this->squareIndexBuffer.GetAddressOf());
		HSystem::ThrowIfFailed(hr);

		// create Vertex buffer for background
		D3D11_BUFFER_DESC vertexBufferDesc = { 0 };
		vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		vertexBufferDesc.ByteWidth = sizeof(DirectX::XMFLOAT3) * 4;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA vertexBufferData;
		ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
		vertexBufferData.pSysMem = backgroundTexturePositions;

		hr = d3dDev->CreateBuffer(&vertexBufferDesc, &vertexBufferData, this->squareVertBuffer.GetAddressOf());
		HSystem::ThrowIfFailed(hr);

		//Create Texture buffer for background
		D3D11_BUFFER_DESC textureBufferDesc = { 0 };
		textureBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		textureBufferDesc.ByteWidth = sizeof(DirectX::XMFLOAT2) * 4;
		textureBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		textureBufferDesc.CPUAccessFlags = 0;
		textureBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA textureBufferData;
		ZeroMemory(&textureBufferData, sizeof(textureBufferData));
		textureBufferData.pSysMem = backgroundTextureTexCoords;

		hr = d3dDev->CreateBuffer(&textureBufferDesc, &textureBufferData, this->squareTextureBuffer.GetAddressOf());
		HSystem::ThrowIfFailed(hr);

		// create Constant buffer for background
		D3D11_BUFFER_DESC bgConstBuff = { 0 };
		bgConstBuff.Usage = D3D11_USAGE_DEFAULT;
		bgConstBuff.ByteWidth = sizeof(ConstantBufferData);
		bgConstBuff.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bgConstBuff.CPUAccessFlags = 0;
		bgConstBuff.MiscFlags = 0;

		hr = d3dDev->CreateBuffer(&bgConstBuff, nullptr, this->bgConstantBuffer.GetAddressOf());
		HSystem::ThrowIfFailed(hr);

		// initialize gesture recognizer
		this->gestureHelper = ref new GestureHelper();

		this->gestureHelper->MoveUpdated = [=](const DirectX::XMFLOAT2 &moveVec, const DirectX::XMFLOAT2 &newPos) { this->ProcessMove(moveVec, newPos); };
		this->gestureHelper->ZoomUpdated = [=](float scale, float x, float y) { this->ProcessZoom(scale, x, y); };
		this->gestureHelper->RotateUpdated = [=](float angle, float x, float y) {this->ProcessRotating(angle, x, y); };
		this->gestureHelper->ManipulationStarted = [=](float x, float y) { this->ManipulationStarted(x, y); };
		this->gestureHelper->ManipulationCompleted = [=](const DirectX::XMFLOAT2 &pos) { this->ManipulationCompleted(pos); };
		this->gestureHelper->Tapped = [=](int tapCount, float x, float y) {this->ProcessTap(tapCount, x, y); };

		/*this->showSlidersEvent;
		this->horizontalManipulationChanged;
		this->verticalManipulationChanged;*/

		this->tapOnSphere = false;
		auto tmpVector = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		DirectX::XMStoreFloat4(&this->prevPoint, tmpVector);

		std::unique_lock<std::mutex> lkInit(this->initializedMtx);
		this->initialized = true;
		this->inititalizedCv.notify_all();
	});
}

void EarthRendererNative::Shutdown() {
	this->indexBuffer.Get()->Release();
	this->vertexBuffer.Get()->Release();
	this->modelTextureBuffer.Get()->Release();
	this->constantBuffer.Get()->Release();

	this->squareIndexBuffer.Get()->Release();
	this->squareVertBuffer.Get()->Release();
	this->squareTextureBuffer.Get()->Release();
	this->bgConstantBuffer.Get()->Release();
}

void EarthRendererNative::CreateDeviceDependentResources() {

}

void EarthRendererNative::ReleaseDeviceDependentResources() {

}

void EarthRendererNative::CreateSizeDependentResources() {
	this->WaitForInitialization();

	auto dxDev = this->dx->Get();
	concurrency::critical_section::scoped_lock lk(this->dataCs);

	auto size = dxDev->GetLogicalSize();
	auto view = DirectX::XMMatrixLookAtLH(DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), DirectX::XMVectorSet(0, 0, 10, 0), DirectX::g_XMIdentityR1);

	// for model
	DirectX::XMStoreFloat4x4(&this->constantBufferData.model, DirectX::XMMatrixTranspose(DirectX::XMMatrixTranslation(0, 0, 2)));
	DirectX::XMStoreFloat4x4(&this->constantBufferData.view, DirectX::XMMatrixTranspose(view));
	DirectX::XMStoreFloat4x4(&this->constantBufferData.projection, DirectX::XMMatrixIdentity());

	{
		auto outputSize = dxDev->GetOutputSize();
		float aspectRatio = outputSize.Width / outputSize.Height;
		float fovAngleY = 90.0f * DirectX::XM_PI / 180.0f;

		DirectX::XMMATRIX orthoMatrix = DirectX::XMMatrixOrthographicLH(aspectRatio * 15.0f, 15.0f, 0.1f, 100.0f);
		DirectX::XMFLOAT4X4 orientation = dxDev->GetOrientationTransform3D();
		DirectX::XMMATRIX orientationMatrix = XMLoadFloat4x4(&orientation);

		DirectX::XMStoreFloat4x4(
			&this->constantBufferData.projection,
			XMMatrixTranspose(orthoMatrix * orientationMatrix)
			);
	}

	// for background
	DirectX::XMStoreFloat4x4(&this->bgConstantBufferData.model, DirectX::XMMatrixTranspose(DirectX::XMMatrixTranslation(0, 0, 2)));
	DirectX::XMStoreFloat4x4(&this->bgConstantBufferData.view, DirectX::XMMatrixTranspose(view));
	DirectX::XMStoreFloat4x4(&this->bgConstantBufferData.projection, DirectX::XMMatrixIdentity());

	{
		auto outputSize = dxDev->GetOutputSize();
		float aspectRatio = outputSize.Width / outputSize.Height;
		float fovAngleY = 90.0f * DirectX::XM_PI / 180.0f;

		DirectX::XMMATRIX perspectiveMatrix = DirectX::XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, 0.01f, 100.0f);
		DirectX::XMFLOAT4X4 orientation = dxDev->GetOrientationTransform3D();
		DirectX::XMMATRIX orientationMatrix = XMLoadFloat4x4(&orientation);

		DirectX::XMStoreFloat4x4(
			&this->bgConstantBufferData.projection,
			XMMatrixTranspose(perspectiveMatrix * orientationMatrix)
			);
	}
}

void EarthRendererNative::OnRenderThreadStart() {

}

void EarthRendererNative::OnRenderThreadEnd() {

}

void EarthRendererNative::Update(const DX::StepTimer &timer) {
	concurrency::critical_section::scoped_lock lk(this->dataCs);
	// rotating till song playing
	if (this->earthRotationEnabled) {
		float currentAngle = (float)timer.GetElapsedSeconds() * HORIZONTAL_ROTATION_FACTOR;

		auto matrixRotationTmp = DirectX::XMLoadFloat4x4(&this->matrixRotation);
		matrixRotationTmp = DirectX::XMMatrixMultiply(matrixRotationTmp, DirectX::XMMatrixRotationRollPitchYaw(0.0f, DirectX::XMConvertToRadians(currentAngle), 0.0f));
		DirectX::XMStoreFloat4x4(&this->matrixRotation, matrixRotationTmp);
	}
}

void EarthRendererNative::Render() {
	this->WaitForInitialization();

	auto dxDev = this->dx->Get();
	auto d3dCtx = dxDev->GetD3DDeviceContext();
	concurrency::critical_section::scoped_lock lk(this->dataCs);

	if (this->modelLoaded) {
		auto matrixRotationTmp = DirectX::XMLoadFloat4x4(&this->matrixRotation);
		DirectX::XMMATRIX modelMatrix = DirectX::XMMatrixMultiply(matrixRotationTmp, DirectX::XMMatrixTranslation(0, 0, 10));

		modelMatrix = DirectX::XMMatrixMultiply(
			modelMatrix,
			DirectX::XMMatrixScaling(this->scale, this->scale, this->scale));

		DirectX::XMStoreFloat4x4(&this->constantBufferData.model, DirectX::XMMatrixTranspose(modelMatrix));

		{
			UINT stride = sizeof(DirectX::XMFLOAT3);
			UINT offset = 0;

			d3dCtx->IASetVertexBuffers(
				0,
				1,
				this->vertexBuffer.GetAddressOf(),
				&stride,
				&offset
				);

			d3dCtx->IASetVertexBuffers(
				1,
				1,
				this->normalBuffer.GetAddressOf(),
				&stride,
				&offset
				);

			stride = sizeof(DirectX::XMFLOAT2);
			d3dCtx->IASetVertexBuffers(
				2,
				1,
				this->modelTextureBuffer.GetAddressOf(),
				&stride,
				&offset
				);
		}

		d3dCtx->IASetIndexBuffer(
			this->indexBuffer.Get(),
			DXGI_FORMAT_R32_UINT, // Each index is one 16-bit unsigned integer (short).
			0
			);

		d3dCtx->IASetInputLayout(this->inputLayout.Get());
		d3dCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		d3dCtx->VSSetShader(this->vertexShader.Get(), 0, 0);
		d3dCtx->VSSetConstantBuffers(0, 1, this->constantBuffer.GetAddressOf());

		d3dCtx->PSSetShaderResources(0, 1, this->modelTextureView.GetAddressOf());
		d3dCtx->PSSetSamplers(0, 1, this->sampler.GetAddressOf());
		d3dCtx->PSSetShader(this->pixelShader.Get(), 0, 0);

		d3dCtx->UpdateSubresource(this->constantBuffer.Get(), 0, nullptr, &this->constantBufferData, 0, 0);

		d3dCtx->DrawIndexed(this->indexCount, 0, 0);

		// draw background
		{
			{
				UINT stride2 = sizeof(DirectX::XMFLOAT3);
				UINT offset2 = 0;

				d3dCtx->IASetIndexBuffer(this->squareIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
				d3dCtx->IASetVertexBuffers(0, 1, this->squareVertBuffer.GetAddressOf(), &stride2, &offset2);

				stride2 = sizeof(DirectX::XMFLOAT2);
				d3dCtx->IASetVertexBuffers(1, 1, this->squareTextureBuffer.GetAddressOf(), &stride2, &offset2);
			}

			d3dCtx->IASetInputLayout(this->inputLayoutBackground.Get());

			d3dCtx->VSSetShader(this->squareVertexShader.Get(), 0, 0);
			d3dCtx->PSSetShader(this->squarePixelShader.Get(), 0, 0);

			d3dCtx->VSSetConstantBuffers(0, 1, this->bgConstantBuffer.GetAddressOf());

			d3dCtx->PSSetShaderResources(0, 1, this->backgroundTextureView.GetAddressOf());
			d3dCtx->PSSetSamplers(0, 1, this->sampler.GetAddressOf());

			d3dCtx->UpdateSubresource(this->bgConstantBuffer.Get(), 0, nullptr, &this->bgConstantBufferData, 0, 0);

			d3dCtx->DrawIndexed(6, 0, 0);
		}
	}
}

void EarthRendererNative::PointerPressed(Windows::UI::Input::PointerPoint ^ppt) {
	auto point = ppt->Position;
	this->gestureHelper->ProcessPress(ppt);
}

void EarthRendererNative::PointerMoved(Windows::UI::Input::PointerPoint ^ppt) {
	auto point = ppt->Position;
	this->gestureHelper->ProcessMove(ppt);
}

void EarthRendererNative::PointerReleased(Windows::UI::Input::PointerPoint ^ppt) {
	this->gestureHelper->ProcessRelease(ppt);
}

void EarthRendererNative::PointerWheelChanged(Windows::UI::Input::PointerPoint ^ppt) {
	const float ZoomSize = 1.01f;
	auto delta = ppt->Properties->MouseWheelDelta;

	if (delta > 0.0f && this->scale <= 5.0f) {
		this->scale *= ZoomSize;
	}
	else if (delta < 0.0f && this->scale > 1.0f) {
		this->scale /= ZoomSize;
	}
}

void EarthRendererNative::LoadModel(const std::string &path) {
	this->WaitForInitialization();

	auto dxDev = this->dx->Get();
	auto d3dDev = dxDev->GetD3DDevice();
	auto d3dCtx = dxDev->GetD3DDeviceContext();
	concurrency::critical_section::scoped_lock lk(this->dataCs);

	HRESULT hr = S_OK;
	Assimp::Importer modelImporter;
	const aiScene *scene = NULL;
	const aiMesh *mesh = NULL;
	const aiFace *faces = NULL;
	int indicesNum = 0;
	int verticesNum = 0;
	std::vector<uint32_t> materialIndices;

	scene = modelImporter.ReadFile(path, aiProcess_Triangulate | aiProcess_GenNormals);

	if (scene) {
		mesh = scene->mMeshes[0];

		if (mesh) {
			if (mesh->HasFaces()) {
				faces = mesh->mFaces;
				indicesNum = mesh->mNumFaces * 3;
				materialIndices.resize(indicesNum);

				for (size_t i = 0; i < mesh->mNumFaces; i++) {
					if (faces && faces[i].mNumIndices == 3) {
						for (size_t j = 0; j < 3; j++) {
							materialIndices[i * 3 + j] = faces[i].mIndices[j];
						}
					}
					else {
						H::System::DebuggerBreak();
					}
				}
			}

			if (mesh->HasPositions()) {
				verticesNum = mesh->mNumVertices;

				this->modelPoints.Vertex.resize(verticesNum);
				this->modelPoints.Normal.resize(verticesNum);
				this->modelPoints.TextureCoord.resize(verticesNum);

				for (int i = 0; i < verticesNum; i++) {
					this->modelPoints.Vertex[i] = DirectX::XMFLOAT3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
				}

				if (mesh->HasNormals()) {
					for (int i = 0; i < verticesNum; i++) {
						DirectX::XMVECTOR xvNormal = DirectX::XMLoadFloat3((DirectX::XMFLOAT3 *)&mesh->mNormals[i]);
						xvNormal = DirectX::XMVector3Normalize(xvNormal);

						DirectX::XMStoreFloat3(&this->modelPoints.Normal[i], xvNormal);
					}
				}

				if (mesh->HasTextureCoords(0)) {
					for (int i = 0; i < verticesNum; i++) {
						this->modelPoints.TextureCoord[i] = DirectX::XMFLOAT2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
					}
				}
			}

			this->indexCount = mesh->mNumFaces * 3;

			//Create Index buffer
			D3D11_BUFFER_DESC indexBufferDesc = { 0 };
			indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
			indexBufferDesc.ByteWidth = sizeof(DWORD) * this->indexCount;
			indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			indexBufferDesc.CPUAccessFlags = 0;
			indexBufferDesc.MiscFlags = 0;
			indexBufferDesc.StructureByteStride = 0;

			D3D11_SUBRESOURCE_DATA idxInitData;
			idxInitData.pSysMem = materialIndices.data();

			hr = d3dDev->CreateBuffer(&indexBufferDesc, &idxInitData, this->indexBuffer.GetAddressOf());
			HSystem::ThrowIfFailed(hr);

			//Create Vertex Buffer
			D3D11_BUFFER_DESC vertexBufferDesc = { 0 };
			vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
			vertexBufferDesc.ByteWidth = sizeof(DirectX::XMFLOAT3) * verticesNum;
			vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vertexBufferDesc.CPUAccessFlags = 0;
			vertexBufferDesc.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA vertexBufferData;
			ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
			vertexBufferData.pSysMem = this->modelPoints.Vertex.data();

			hr = d3dDev->CreateBuffer(&vertexBufferDesc, &vertexBufferData, this->vertexBuffer.GetAddressOf());
			HSystem::ThrowIfFailed(hr);

			//Create Normal Buffer
			D3D11_BUFFER_DESC normalBufferDesc = { 0 };
			normalBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
			normalBufferDesc.ByteWidth = sizeof(DirectX::XMFLOAT3) * this->modelPoints.Normal.size();
			normalBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			normalBufferDesc.CPUAccessFlags = 0;
			normalBufferDesc.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA normalBufferData;
			ZeroMemory(&normalBufferData, sizeof(normalBufferData));
			normalBufferData.pSysMem = this->modelPoints.Normal.data();

			hr = d3dDev->CreateBuffer(&normalBufferDesc, &normalBufferData, this->normalBuffer.GetAddressOf());
			HSystem::ThrowIfFailed(hr);

			//Create Texture Buffer 
			D3D11_BUFFER_DESC textureBufferDesc = { 0 };
			textureBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			textureBufferDesc.ByteWidth = sizeof(DirectX::XMFLOAT2) * this->modelPoints.TextureCoord.size();
			textureBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			textureBufferDesc.CPUAccessFlags = 0;
			textureBufferDesc.MiscFlags = 0;

			for (auto &coord : this->modelPoints.TextureCoord) {
				coord.x *= -1;
				coord.y *= -1;
			}

			D3D11_SUBRESOURCE_DATA textureBufferData;
			ZeroMemory(&textureBufferData, sizeof(textureBufferData));
			textureBufferData.pSysMem = this->modelPoints.TextureCoord.data();

			hr = d3dDev->CreateBuffer(&textureBufferDesc, &textureBufferData, this->modelTextureBuffer.GetAddressOf());
			HSystem::ThrowIfFailed(hr);

			this->modelLoaded = true;
		}
		else {
			H::System::DebuggerBreak();
		}
	}
	else {
		H::System::DebuggerBreak();
	}
}

void EarthRendererNative::LoadModelTexture(const std::wstring &path) {
	this->WaitForInitialization();

	auto dxDev = this->dx->Get();
	auto d3dDev = dxDev->GetD3DDevice();
	auto d3dCtx = dxDev->GetD3DDeviceContext();
	concurrency::critical_section::scoped_lock lk(this->dataCs);

	HRESULT hr = S_OK;
	auto sizeRT = dxDev->GetRendertargetSize();

	Platform::String ^pathTmp = ref new Platform::String(path.c_str());
	auto imgFile = H::System::PerformSync(Windows::ApplicationModel::Package::Current->InstalledLocation->GetFileAsync(pathTmp)).second;
	auto imgStream = H::System::PerformSync(imgFile->OpenAsync(Windows::Storage::FileAccessMode::Read)).second;
	imgStream->Seek(0);

	ImageUtils imgUtils;
	imgUtils.Initialize();
	auto imgDecoder = imgUtils.CreateDecoder(imgStream);
	auto imgDecoderFrame = imgUtils.CreateFrameForDecode(imgDecoder.Get());
	auto imgFrameSize = imgUtils.GetFrameSize(imgDecoderFrame.Get());
	auto frameByteSize = imgUtils.GetFrameByteSize(imgDecoderFrame.Get());

	/*Microsoft::WRL::ComPtr<IWICDdsDecoder> ddsDecoder;
	Microsoft::WRL::ComPtr<IWICDdsFrameDecode> ddsFrame;

	hr = imgDecoder.As(&ddsDecoder);
	H::System::ThrowIfFailed(hr);

	hr = imgDecoderFrame.As(&ddsFrame);
	H::System::ThrowIfFailed(hr);

	WICDdsParameters ddsParams;

	hr = ddsDecoder->GetParameters(&ddsParams);
	H::System::ThrowIfFailed(hr);*/

	std::vector<uint8_t> frameData;
	frameData.resize(frameByteSize);
	imgUtils.DecodePixels(imgDecoderFrame.Get(), frameByteSize, frameData.data());

	D3D11_TEXTURE2D_DESC texDesc = { 0 };
	texDesc.Width = imgFrameSize.x;
	texDesc.Height = imgFrameSize.y;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	texDesc.BindFlags =
		D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE |
		D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA textInitData;
	ZeroMemory(&textInitData, sizeof(textInitData));
	textInitData.pSysMem = frameData.data();
	textInitData.SysMemPitch = imgFrameSize.x * 4;

	hr = d3dDev->CreateTexture2D(&texDesc, &textInitData, this->modelTexture.GetAddressOf());
	H::System::ThrowIfFailed(hr);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	hr = d3dDev->CreateShaderResourceView(this->modelTexture.Get(), &srvDesc, this->modelTextureView.GetAddressOf());
	H::System::ThrowIfFailed(hr);
}

void EarthRendererNative::LoadBackgroundTexture(const std::wstring &path) {
	this->WaitForInitialization();

	auto dxDev = this->dx->Get();
	auto d3dDev = dxDev->GetD3DDevice();
	auto d3dCtx = dxDev->GetD3DDeviceContext();
	concurrency::critical_section::scoped_lock lk(this->dataCs);

	HRESULT hr = S_OK;
	auto sizeRT = dxDev->GetRendertargetSize();

	Platform::String ^pathTmp = ref new Platform::String(path.c_str());
	auto imgFile = H::System::PerformSync(Windows::ApplicationModel::Package::Current->InstalledLocation->GetFileAsync(pathTmp)).second;
	auto imgStream = H::System::PerformSync(imgFile->OpenAsync(Windows::Storage::FileAccessMode::Read)).second;
	imgStream->Seek(0);

	ImageUtils imgUtils;
	imgUtils.Initialize();
	auto imgDecoder = imgUtils.CreateDecoder(imgStream);
	auto imgDecoderFrame = imgUtils.CreateFrameForDecode(imgDecoder.Get());
	auto imgFrameSize = imgUtils.GetFrameSize(imgDecoderFrame.Get());
	auto frameByteSize = imgUtils.GetFrameByteSize(imgDecoderFrame.Get());

	/*Microsoft::WRL::ComPtr<IWICDdsDecoder> ddsDecoder;
	Microsoft::WRL::ComPtr<IWICDdsFrameDecode> ddsFrame;

	hr = imgDecoder.As(&ddsDecoder);
	H::System::ThrowIfFailed(hr);

	hr = imgDecoderFrame.As(&ddsFrame);
	H::System::ThrowIfFailed(hr);

	WICDdsParameters ddsParams;

	hr = ddsDecoder->GetParameters(&ddsParams);
	H::System::ThrowIfFailed(hr);*/

	std::vector<uint8_t> frameData;
	frameData.resize(frameByteSize);
	imgUtils.DecodePixels(imgDecoderFrame.Get(), frameByteSize, frameData.data());

	D3D11_TEXTURE2D_DESC texDesc = { 0 };
	texDesc.Width = imgFrameSize.x;
	texDesc.Height = imgFrameSize.y;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	texDesc.BindFlags =
		D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE |
		D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA textInitData;
	ZeroMemory(&textInitData, sizeof(textInitData));
	textInitData.pSysMem = frameData.data();
	textInitData.SysMemPitch = imgFrameSize.x * 4;

	hr = d3dDev->CreateTexture2D(&texDesc, &textInitData, this->backgroundTexture.GetAddressOf());
	H::System::ThrowIfFailed(hr);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	hr = d3dDev->CreateShaderResourceView(this->backgroundTexture.Get(), &srvDesc, this->backgroundTextureView.GetAddressOf());
	H::System::ThrowIfFailed(hr);
}

bool EarthRendererNative::GetEarthRotationEnabled() {
	concurrency::critical_section::scoped_lock lk(this->externDataCs);
	return this->earthRotationEnabled;
}

void EarthRendererNative::SetEarthRotationEnabled(bool v) {
	concurrency::critical_section::scoped_lock lk(this->externDataCs);
	this->earthRotationEnabled = v;
}

void EarthRendererNative::ResetRotationAngles() {
	concurrency::critical_section::scoped_lock lk(this->externDataCs);
	// song state
	this->rotationAngleHorizontal = 0.0f;
	this->rotationAngleVertical = 0.0f;
	this->angleForRewinding = 0.0f;
	this->angleForVolumeChange = 0.0f;

	// model state
	DirectX::XMStoreFloat4x4(&this->matrixRotation, DirectX::XMMatrixRotationRollPitchYaw(0.0f, DirectX::XMConvertToRadians(250.0f), 0.0f));
}

void EarthRendererNative::SetHorisontalRotationAngle(float angle) {
	concurrency::critical_section::scoped_lock lk(this->externDataCs);
	auto matrixRotationTmp = DirectX::XMLoadFloat4x4(&this->matrixRotation);
	matrixRotationTmp = DirectX::XMMatrixMultiply(matrixRotationTmp, DirectX::XMMatrixRotationRollPitchYaw(0.0f, DirectX::XMConvertToRadians(angle), 0.0f));
	DirectX::XMStoreFloat4x4(&this->matrixRotation, matrixRotationTmp);
}

void EarthRendererNative::SetVerticalRotationAngle(float angle) {
	concurrency::critical_section::scoped_lock lk(this->externDataCs);
	auto matrixRotationTmp = DirectX::XMLoadFloat4x4(&this->matrixRotation);
	matrixRotationTmp = DirectX::XMMatrixMultiply(matrixRotationTmp, DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(angle), 0.0f, 0.0f));
	DirectX::XMStoreFloat4x4(&this->matrixRotation, matrixRotationTmp);
}

bool EarthRendererNative::GetManipulationMode() {
	return this->managingByEarthManipulations;
}

void EarthRendererNative::SetPlayingMode(bool v) {
	concurrency::critical_section::scoped_lock lk(this->externDataCs);
	this->isSongPlayingNow = v;
}

void EarthRendererNative::SetSongDuration(double duration) {

}

void EarthRendererNative::SetSongCurrentPosition(double pos) {

}

void EarthRendererNative::WaitForInitialization() {
	std::unique_lock<std::mutex> lk(this->initializedMtx);

	while (!this->initialized) {
		this->inititalizedCv.wait(lk);
	}
}

void EarthRendererNative::ProcessMove(const DirectX::XMFLOAT2 &moveVec, const DirectX::XMFLOAT2 &newPos) {
	if (this->tapOnSphere) {
		Windows::Foundation::Size rtSize;
		DirectX::XMMATRIX proj;

		{
			auto dxDev = this->dx->Get();
			rtSize = dxDev->GetLogicalSize();
			proj = DirectX::XMLoadFloat4x4(&this->projection);
		}

		DirectX::XMVECTOR earthPosition = DirectX::XMVectorSet(0.0f, 0.0f, 2.0f, 1.0f);
		DirectX::XMVECTOR nextPoint = DirectX::XMVectorSet(newPos.x, newPos.y, 0.0f, 1.0f);
		DirectX::XMVECTOR prevPointVec = DirectX::XMLoadFloat4(&this->prevPoint);

		nextPoint = DirectX::XMVector3Unproject(nextPoint, 0.0f, 0.0f,
			rtSize.Width, rtSize.Height, 0.0f, 1.0f,
			proj, DirectX::XMMatrixIdentity(), DirectX::XMMatrixIdentity());

		auto vectorFromPrevPointToCenter = DirectX::XMVectorSubtract(earthPosition, prevPointVec);
		auto vectorFromCurrPointToCenter = DirectX::XMVectorSubtract(earthPosition, nextPoint);
		auto angleBetweenVectors = DirectX::XMVector3AngleBetweenVectors(vectorFromPrevPointToCenter, vectorFromCurrPointToCenter);

		float angleInDeg = DirectX::XMConvertToDegrees(angleBetweenVectors.m128_f32[0]);
		auto swapDirection = DirectX::XMVectorSubtract(nextPoint, prevPointVec);

		// not works now
		if (/*this->managingByEarthManipulations*/false) {
			this->angleForRewinding -= swapDirection.m128_f32[0] * 100;
			this->angleForVolumeChange -= swapDirection.m128_f32[1] * 100;

			if (std::abs(swapDirection.m128_f32[0]) > std::abs(swapDirection.m128_f32[1])) {
				this->rotationAngleHorizontal -= swapDirection.m128_f32[0] * 100;
			}
			else if (std::abs(swapDirection.m128_f32[1]) > std::abs(swapDirection.m128_f32[0])) {
				this->rotationAngleVertical -= swapDirection.m128_f32[1] * 100;
			}

			if (this->rotationAngleHorizontal > 0) {
				if (this->rotationAngleHorizontal > this->HORIZONTAL_ANGLE_LIMITER_TOP) {
					this->angleForRewinding = 360.0f;
					this->rotationAngleHorizontal = this->HORIZONTAL_ANGLE_LIMITER_TOP;
				}
				if (this->rotationAngleHorizontal < this->HORIZONTAL_ANGLE_LIMITER_BOTTOM) {
					this->angleForRewinding = 0.0f;
					this->rotationAngleHorizontal = this->HORIZONTAL_ANGLE_LIMITER_BOTTOM;
				}
			}
			else {
				if (this->rotationAngleHorizontal < this->HORIZONTAL_ANGLE_LIMITER_TOP) {
					this->angleForRewinding = -360.0f;
					this->rotationAngleHorizontal = this->HORIZONTAL_ANGLE_LIMITER_TOP;
				}
				if (this->rotationAngleHorizontal > this->HORIZONTAL_ANGLE_LIMITER_BOTTOM) {
					this->angleForRewinding = 0.0f;
					this->rotationAngleHorizontal = this->HORIZONTAL_ANGLE_LIMITER_BOTTOM;
				}
			}
		}
		else {
			if (std::abs(swapDirection.m128_f32[0]) > std::abs(swapDirection.m128_f32[1])) {
				this->rotationAngleHorizontal -= swapDirection.m128_f32[0] * 100;
			}
			else if (std::abs(swapDirection.m128_f32[1]) > std::abs(swapDirection.m128_f32[0])) {
				this->rotationAngleVertical -= swapDirection.m128_f32[1] * 100;
			}
		}

		if (this->rotationAngleVertical > 45.0f) {
			this->rotationAngleVertical = 45.0f;
			this->angleForVolumeChange = 55.0f;
		}
		if (this->rotationAngleVertical < -45.0f) {
			this->rotationAngleVertical = -45.0f;
			this->angleForVolumeChange = 0.0f;
		}

		auto matrixRotationTmp = DirectX::XMLoadFloat4x4(&this->matrixRotation);
		auto preRotate = DirectX::XMMatrixRotationRollPitchYaw(0.0f, DirectX::XMConvertToRadians(250.0f), 0.0f);

		matrixRotationTmp = DirectX::XMMatrixMultiply(preRotate, DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(this->rotationAngleHorizontal)));
		matrixRotationTmp = DirectX::XMMatrixMultiply(matrixRotationTmp, DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(-this->rotationAngleVertical)));

		DirectX::XMStoreFloat4x4(&this->matrixRotation, matrixRotationTmp);
		DirectX::XMStoreFloat4(&this->prevPoint, nextPoint);

		if (this->managingByEarthManipulations) {
			if (this->horizontalManipulationChanged) {
				this->horizontalManipulationChanged(this->angleForRewinding);
			}
			if (this->verticalManipulationChanged) {
				this->verticalManipulationChanged(this->angleForVolumeChange);
			}
		}
	}
}

void EarthRendererNative::ProcessZoom(float scale, float x, float y) {
	this->scale *= scale;
}

void EarthRendererNative::ProcessRotating(float angle, float x, float y) {
	int s = 3;
}

void EarthRendererNative::ManipulationStarted(float x, float y) {
	auto dxDev = this->dx->Get();
	auto rtSize = dxDev->GetLogicalSize();
	auto proj = DirectX::XMLoadFloat4x4(&this->projection);

	DirectX::XMVECTOR tapPoint0 = DirectX::XMVectorSet(x, y, 0.0f, 1.0f);
	DirectX::XMVECTOR tapPoint1 = DirectX::XMVectorSet(x, y, 1.0f, 1.0f);
	DirectX::XMVECTOR direction = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);
	DirectX::XMVECTOR earthPosition = DirectX::XMVectorSet(0.0f, 0.0f, 2.0f, 1.0f);	// not sure that its right, but its working :)

	tapPoint0 = DirectX::XMVector3Unproject(tapPoint0, 0.0f, 0.0f,
		rtSize.Width, rtSize.Height, 0.0f, 1.0f,
		proj, DirectX::XMMatrixIdentity(), DirectX::XMMatrixIdentity());	// ray origin
	tapPoint1 = DirectX::XMVector3Unproject(tapPoint1, 0.0f, 0.0f,
		rtSize.Width, rtSize.Height, 0.0f, 1.0f,
		proj, DirectX::XMMatrixIdentity(), DirectX::XMMatrixIdentity());

	direction = DirectX::XMVectorSubtract(tapPoint1, tapPoint0);			// ray direction

	DirectX::XMVECTOR hitPoint = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	DirectX::XMVECTOR normal = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	float distance = 0.0f;

	this->tapOnSphere = this->Intersect(direction, tapPoint0, earthPosition, 0.5f, hitPoint, distance, normal);	// not sure that Earth radius is right, but its working :)
	DirectX::XMStoreFloat4(&this->prevPoint, hitPoint);

	if (this->managingByEarthManipulations) {
		if (this->onManipulationsStarted) {
			this->onManipulationsStarted();
		}
	}
}

void EarthRendererNative::ManipulationCompleted(const DirectX::XMFLOAT2 &pos) {
	this->tapOnSphere = false;

	if (this->managingByEarthManipulations) {
		if (this->onManipulationsEnded) {
			this->onManipulationsEnded();
		}
	}
}

void EarthRendererNative::ProcessTap(int tapCount, float x, float y) {
	auto dxDev = this->dx->Get();
	auto rtSize = dxDev->GetLogicalSize();
	auto proj = DirectX::XMLoadFloat4x4(&this->projection);

	DirectX::XMVECTOR tapPoint0 = DirectX::XMVectorSet(x, y, 0.0f, 1.0f);
	DirectX::XMVECTOR tapPoint1 = DirectX::XMVectorSet(x, y, 1.0f, 1.0f);
	DirectX::XMVECTOR direction = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);
	DirectX::XMVECTOR earthPosition = DirectX::XMVectorSet(0.0f, 0.0f, 2.0f, 1.0f);	// not sure that its right, but its working :)

	tapPoint0 = DirectX::XMVector3Unproject(tapPoint0, 0.0f, 0.0f,
		rtSize.Width, rtSize.Height, 0.0f, 1.0f,
		proj, DirectX::XMMatrixIdentity(), DirectX::XMMatrixIdentity());	// ray origin
	tapPoint1 = DirectX::XMVector3Unproject(tapPoint1, 0.0f, 0.0f,
		rtSize.Width, rtSize.Height, 0.0f, 1.0f,
		proj, DirectX::XMMatrixIdentity(), DirectX::XMMatrixIdentity());

	direction = DirectX::XMVectorSubtract(tapPoint1, tapPoint0);			// ray direction

	DirectX::XMVECTOR hitPoint = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	DirectX::XMVECTOR normal = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	float distance = 0.0f;

	this->tapOnSphere = this->Intersect(direction, tapPoint0, earthPosition, 0.5f, hitPoint, distance, normal);	// not sure that Earth radius is right, but its working :)
	DirectX::XMStoreFloat4(&this->prevPoint, hitPoint);

	this->tapCount++;
	this->tapTimer->destroy(this->tapTimerPeriod);

	this->tapTimerPeriod = this->tapTimer->create(100, 0, [=]() {
		int tapCountTmp = this->tapCount;

		if (this->tapOnSphere && tapCountTmp == 2 && this->isSongPlayingNow) {
			this->managingByEarthManipulations = true;
			this->HORIZONTAL_ANGLE_LIMITER_BOTTOM = this->rotationAngleHorizontal;
			this->HORIZONTAL_ANGLE_LIMITER_TOP = (this->rotationAngleHorizontal >= 0.0f) ? (this->rotationAngleHorizontal + 360.0f) : (this->rotationAngleHorizontal - 360.0f);

			if (this->onProcessDoubleTap) {
				this->onProcessDoubleTap();
			}
		}
		else {
			this->managingByEarthManipulations = false;

			if (this->onProcessTap) {
				this->onProcessTap();
			}
		}

		this->tapCount = 0;
	});
}

inline bool EarthRendererNative::Intersect(const DirectX::XMVECTOR &rayDir, const DirectX::XMVECTOR &rayOrig,
	const DirectX::XMVECTOR &earthPos, float earthRad,
	DirectX::XMVECTOR &hitPoint, float &distance, DirectX::XMVECTOR &normal)
{
	float a = DirectX::XMVector3Dot(rayDir, rayDir).m128_f32[0];
	float b = DirectX::XMVector3Dot(rayDir, DirectX::XMVectorScale(DirectX::XMVectorSubtract(rayOrig, earthPos), 2.0f)).m128_f32[0];
	float c = DirectX::XMVector3Dot(earthPos, earthPos).m128_f32[0] + DirectX::XMVector3Dot(rayOrig, rayOrig).m128_f32[0] -
		DirectX::XMVectorScale(DirectX::XMVector3Dot(rayOrig, earthPos), 2.0f).m128_f32[0] - earthRad * earthRad;
	float D = b * b + (-4.0f) * a * c;

	// If ray can not intersect then stop
	if (D < 0) {
		return false;
	}

	D = std::sqrtf(D);

	// Ray can intersect the sphere, solve the closer hitpoint
	float t = (-0.5f) * (b + D) / a;

	if (t > 0.0f) {
		distance = std::sqrtf(a) * t;
		hitPoint = DirectX::XMVectorAdd(rayOrig, DirectX::operator *(rayDir, t));
		normal = DirectX::XMVectorScale(DirectX::XMVectorSubtract(hitPoint, earthPos), 1.0f / earthRad);
	}
	else {
		return false;
	}

	return true;
}
