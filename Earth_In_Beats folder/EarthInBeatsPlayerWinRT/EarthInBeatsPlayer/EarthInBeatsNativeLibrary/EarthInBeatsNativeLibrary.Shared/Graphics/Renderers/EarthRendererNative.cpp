#include "EarthRendererNative.h"
#include "..\Helpers\H.h"
#include "DxRenderingContext.h"

#include <fstream>

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

EarthRendererNative::EarthRendererNative() : initialized(false), modelLoaded(false){
	DirectX::XMStoreFloat4x4(&this->projection, DirectX::XMMatrixIdentity());
}

EarthRendererNative::~EarthRendererNative(){
}

void EarthRendererNative::Initialize(const std::shared_ptr<GuardedDeviceResources> &dx){
	this->initializeTask = concurrency::create_task([=]() {
		this->dx = dx;
		auto dxDev = this->dx->Get();

		concurrency::critical_section::scoped_lock lk(this->dataCs);

		auto d3dDev = dxDev->GetD3DDevice();

		////////
		//create resources

		std::unique_lock<std::mutex> lkInit(this->initializedMtx);
		this->initialized = true;
		this->inititalizedCv.notify_all();
	});
}

void EarthRendererNative::CreateDeviceDependentResources(){

}

void EarthRendererNative::ReleaseDeviceDependentResources(){

}

void EarthRendererNative::CreateSizeDependentResources(){
	this->WaitForInitialization();

	auto dxDev = this->dx->Get();
	concurrency::critical_section::scoped_lock lk(this->dataCs);

	auto size = dxDev->GetLogicalSize();
	//resize model
}

void EarthRendererNative::OnRenderThreadStart(){

}

void EarthRendererNative::OnRenderThreadEnd(){

}

void EarthRendererNative::Update(const DX::StepTimer &timer){

}

void EarthRendererNative::Render(){
	this->WaitForInitialization();

	auto dxDev = this->dx->Get();
	auto d3dCtx = dxDev->GetD3DDeviceContext();
	concurrency::critical_section::scoped_lock lk(this->dataCs);

 	DirectX::XMMATRIX proj = DirectX::XMLoadFloat4x4(&this->projection);
	DirectX::XMMATRIX rotationMatrix = DirectX::XMLoadFloat4x4(&dxDev->GetOrientationTransform3D());
	proj = DirectX::XMMatrixMultiply(proj, rotationMatrix);
	
	
}

void EarthRendererNative::PointerPressed(Windows::UI::Input::PointerPoint ^ppt){

}

void EarthRendererNative::PointerMoved(Windows::UI::Input::PointerPoint ^ppt){

}

void EarthRendererNative::PointerReleased(Windows::UI::Input::PointerPoint ^ppt){

}

void EarthRendererNative::LoadModel(std::string path){
	this->WaitForInitialization();

	auto dxDev = this->dx->Get();
	auto d3dDev = dxDev->GetD3DDevice();
	concurrency::critical_section::scoped_lock lk(this->dataCs);

	Assimp::Importer modelImporter;
	const aiScene *scene = NULL;
	const aiMesh *mesh = NULL;
	const aiFace *faces = NULL;
	int indicesNum = 0;
	int verticesNum = 0;
	std::vector<uint32_t> materialIndices;

	HRESULT hr = S_OK;

	////scene = modelImporter.ReadFile(path, aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType | aiProcess_GenNormals);
	scene = modelImporter.ReadFile(path, aiProcess_Triangulate | aiProcess_GenNormals);

	if (scene){
		mesh = scene->mMeshes[0];

		if (mesh){
			if (mesh->HasFaces()){
				faces = mesh->mFaces;
				indicesNum = mesh->mNumFaces * 3;
				materialIndices.resize(indicesNum);

				for (size_t i = 0; i < mesh->mNumFaces; i++){
					if (faces && faces[i].mNumIndices == 3){
						for (size_t j = 0; j < 3; j++){
							materialIndices[i * 3 + j] = faces[i].mIndices[j];
						}
					}
					else{
						H::System::DebuggerBreak();
					}
				}
			}

			if (mesh->HasPositions()){			
				verticesNum = mesh->mNumVertices;

				this->modelPoints.resize(verticesNum);

				for (int i = 0; i < verticesNum; i++){
					this->modelPoints[i].Vertex = DirectX::XMFLOAT3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
				}

				if (mesh->HasNormals()){
					for (int i = 0; i < verticesNum; i++){
						DirectX::XMVECTOR xvNormal = DirectX::XMLoadFloat3((DirectX::XMFLOAT3 *)&mesh->mNormals[i]);
						xvNormal = DirectX::XMVector3Normalize(xvNormal);

						DirectX::XMStoreFloat3(&this->modelPoints[i].Normal, xvNormal);
					}
				}

				if (mesh->HasTextureCoords(0)) {
					for (int i = 0; i < verticesNum; i++){
						this->modelPoints[i].TextureCoord = DirectX::XMFLOAT2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
					}
				}
			}

			//Create index buffer
			D3D11_BUFFER_DESC indexBufferDesc = { 0 };

			indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
			indexBufferDesc.ByteWidth = sizeof(DWORD) * mesh->mNumFaces * 3;
			indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			indexBufferDesc.CPUAccessFlags = 0;
			indexBufferDesc.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA idxInitData;

			idxInitData.pSysMem = &materialIndices[0];
			hr = d3dDev->CreateBuffer(&indexBufferDesc, &idxInitData, this->indexBuffer.GetAddressOf());
			HSystem::ThrowIfFailed(hr);

			//Create Vertex Buffer
			D3D11_BUFFER_DESC vertexBufferDesc = { 0 };

			vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
			vertexBufferDesc.ByteWidth = sizeof(VertexTextureNormal) * verticesNum;
			vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vertexBufferDesc.CPUAccessFlags = 0;
			vertexBufferDesc.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA vertexBufferData;

			ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
			vertexBufferData.pSysMem = &this->modelPoints[0];
			hr = d3dDev->CreateBuffer(&vertexBufferDesc, &vertexBufferData, this->vertexBuffer.GetAddressOf());
			HSystem::ThrowIfFailed(hr);

			this->modelLoaded = true;
		}
		else{
			H::System::DebuggerBreak();
		}
	}
	else{
		H::System::DebuggerBreak();
	}
}

void EarthRendererNative::WaitForInitialization() {
	std::unique_lock<std::mutex> lk(this->initializedMtx);

	while (!this->initialized) {
		this->inititalizedCv.wait(lk);
	}
}
