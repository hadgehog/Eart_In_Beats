#include "Quad.h"
#include "Helpers\H.h"

Quad::Quad(){
}

Quad::~Quad(){
}

void Quad::Initialize(DX::DeviceResources *devRes, bool useDirectXTexCoords){
	this->InitializeVertexData(devRes, useDirectXTexCoords);
	this->InitializeVertexShaderAndLayout(devRes);
	this->InitializeVertexShaderCBuffer(devRes);
	this->InitializePixelShader(devRes);
	this->InitializeSampler(devRes);
}

void Quad::Draw(DX::DeviceResources *devRes,
	const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> &texSrv,
	const DirectX::XMMATRIX &world,
	const DirectX::XMMATRIX &viewProj)
{
	const uint32_t QuadStride = sizeof(DirectX::XMFLOAT2);
	const uint32_t QuadOffset = 0;
	auto d3dCtx = devRes->GetD3DDeviceContext();

	DirectX::XMMATRIX tmpTransform;
	tmpTransform = DirectX::XMMatrixMultiplyTranspose(world, viewProj);

	d3dCtx->UpdateSubresource(
		this->quadVS_CBuffer.Get(),
		0,
		NULL,
		&tmpTransform,
		0,
		0
		);

	d3dCtx->IASetInputLayout(this->quadLayout.Get());
	d3dCtx->IASetVertexBuffers(0, 1, this->quadVerts.GetAddressOf(), &QuadStride, &QuadOffset);
	d3dCtx->IASetVertexBuffers(1, 1, this->quadTex.GetAddressOf(), &QuadStride, &QuadOffset);
	d3dCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	d3dCtx->VSSetConstantBuffers(0, 1, this->quadVS_CBuffer.GetAddressOf());
	d3dCtx->VSSetShader(this->quadVS.Get(), nullptr, 0);

	d3dCtx->PSSetShader(this->quadPS.Get(), nullptr, 0);
	d3dCtx->PSSetSamplers(0, 1, this->quadSampler.GetAddressOf());
	d3dCtx->PSSetShaderResources(0, 1, texSrv.GetAddressOf());

	d3dCtx->Draw(4, 0);
}

void Quad::InitializeVertexShaderAndLayout(DX::DeviceResources *devRes){
	auto dev = devRes->GetD3DDevice();

	auto vsData = H::System::LoadPackageFile(L"EffectifyWinRT\\QuadVS.cso");

	dev->CreateVertexShader(vsData.data(), vsData.size(), nullptr, this->quadVS.ReleaseAndGetAddressOf());

	D3D11_INPUT_ELEMENT_DESC inputDesc [] = {
		{ "POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 1,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	dev->CreateInputLayout(inputDesc, sizeof(inputDesc) / sizeof(inputDesc[0]),
		vsData.data(), vsData.size(), this->quadLayout.ReleaseAndGetAddressOf());
}

void Quad::InitializeVertexShaderCBuffer(DX::DeviceResources *devRes){
	auto dev = devRes->GetD3DDevice();
	DirectX::XMMATRIX transform = DirectX::XMMatrixIdentity();
	D3D11_BUFFER_DESC bufDesc = {};
	bufDesc.ByteWidth = sizeof(DirectX::XMMATRIX);
	bufDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	bufDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;

	D3D11_SUBRESOURCE_DATA subResData = {};
	subResData.pSysMem = &transform;

	dev->CreateBuffer(&bufDesc, &subResData, this->quadVS_CBuffer.ReleaseAndGetAddressOf());
}

void Quad::InitializePixelShader(DX::DeviceResources *devRes){
	auto dev = devRes->GetD3DDevice();
	auto psData = H::System::LoadPackageFile(L"EffectifyWinRT\\QuadPS.cso");

	dev->CreatePixelShader(psData.data(), psData.size(), nullptr, this->quadPS.ReleaseAndGetAddressOf());
}

void Quad::InitializeVertexData(DX::DeviceResources *devRes, bool useDirectXTexCoords){
	auto dev = devRes->GetD3DDevice();
	DirectX::XMFLOAT2 vertexData [] = {
		DirectX::XMFLOAT2(-0.5f, 0.5f),		//left top
		DirectX::XMFLOAT2(0.5f, 0.5f),		//right top
		DirectX::XMFLOAT2(-0.5f, -0.5f),	//left bottom
		DirectX::XMFLOAT2(0.5f, -0.5f)		//right bottom
	};

	DirectX::XMFLOAT2 texData[4];

	if (useDirectXTexCoords){
		texData[0] = DirectX::XMFLOAT2(0.0f, 0.0f);	//left top
		texData[1] = DirectX::XMFLOAT2(1.0f, 0.0f);	//right top
		texData[2] = DirectX::XMFLOAT2(0.0f, 1.0f);	//left bottom
		texData[3] = DirectX::XMFLOAT2(1.0f, 1.0f);	//right bottom
	}
	else{
		texData[0] = DirectX::XMFLOAT2(0.0f, 1.0f);	//left top
		texData[1] = DirectX::XMFLOAT2(1.0f, 1.0f);	//right top
		texData[2] = DirectX::XMFLOAT2(0.0f, 0.0f);	//left bottom
		texData[3] = DirectX::XMFLOAT2(1.0f, 0.0f);	//right bottom
	}

	D3D11_BUFFER_DESC bufDesc = {};
	bufDesc.ByteWidth = sizeof(vertexData);
	bufDesc.Usage = D3D11_USAGE::D3D11_USAGE_IMMUTABLE;
	bufDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA subResData = {};
	subResData.pSysMem = vertexData;

	dev->CreateBuffer(&bufDesc, &subResData, this->quadVerts.ReleaseAndGetAddressOf());

	bufDesc.ByteWidth = sizeof(texData);
	subResData.pSysMem = texData;

	dev->CreateBuffer(&bufDesc, &subResData, this->quadTex.ReleaseAndGetAddressOf());
}

void Quad::InitializeSampler(DX::DeviceResources *devRes){
	auto dev = devRes->GetD3DDevice();
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = -FLT_MAX;
	samplerDesc.MaxLOD = FLT_MAX;

	dev->CreateSamplerState(&samplerDesc, this->quadSampler.ReleaseAndGetAddressOf());
}