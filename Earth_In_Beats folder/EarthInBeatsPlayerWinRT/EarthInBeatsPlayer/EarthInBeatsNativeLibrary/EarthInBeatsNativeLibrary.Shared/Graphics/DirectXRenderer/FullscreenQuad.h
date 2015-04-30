#pragma once
#include "DirectXCommon\DeviceResources.h"

#include <memory>

class FullscreenQuad{
public:
	FullscreenQuad();
	~FullscreenQuad();

	const DirectX::XMFLOAT4X4 &GetTransform() const;
	void SetTransform(const DirectX::XMFLOAT4X4 &v);

	void Initialize(DX::DeviceResources *devRes);
	void Draw(DX::DeviceResources *devRes, 
		const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> &openGlTexSrv);
private:
	DirectX::XMFLOAT4X4 transform;

	Microsoft::WRL::ComPtr<ID3D11Buffer> quadVerts;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> quadLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> quadVS_CBuffer;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> quadVS;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> quadPS;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> quadSampler;

	void InitializeVertexShaderAndLayout(DX::DeviceResources *devRes);
	void InitializeVertexShaderCBuffer(DX::DeviceResources *devRes);
	void InitializePixelShader(DX::DeviceResources *devRes);
	void InitializeVertexData(DX::DeviceResources *devRes);
	void InitializeSampler(DX::DeviceResources *devRes);
};