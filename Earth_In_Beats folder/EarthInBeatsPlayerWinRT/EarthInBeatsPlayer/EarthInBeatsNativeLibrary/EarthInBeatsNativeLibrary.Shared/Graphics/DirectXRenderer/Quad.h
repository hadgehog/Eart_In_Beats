#pragma once
#include "DirectXCommon\DeviceResources.h"

#include <memory>

class Quad{
public:
	Quad();
	~Quad();

	void Initialize(DX::DeviceResources *devRes, bool useDirectXTexCoords);
	void Draw(DX::DeviceResources *devRes,
		const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> &texSrv,
		const DirectX::XMMATRIX &world,
		const DirectX::XMMATRIX &viewProj);

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> quadVerts;
	Microsoft::WRL::ComPtr<ID3D11Buffer> quadTex;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> quadLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> quadVS_CBuffer;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> quadVS;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> quadPS;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> quadSampler;

	void InitializeVertexShaderAndLayout(DX::DeviceResources *devRes);
	void InitializeVertexShaderCBuffer(DX::DeviceResources *devRes);
	void InitializePixelShader(DX::DeviceResources *devRes);
	void InitializeVertexData(DX::DeviceResources *devRes, bool useDirectXTexCoords);
	void InitializeSampler(DX::DeviceResources *devRes);
};