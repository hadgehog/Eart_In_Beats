#include "MediaRendererSamplerDxResources.h"

#include "..\Helpers\H.h"

MediaRendererSamplerDxResources::MediaRendererSamplerDxResources(const std::shared_ptr<GuardedDeviceResources> &dx)
	: pointSampler(MediaRendererSamplerDxResources::LoadPointSampler, dx),
	linearSampler(MediaRendererSamplerDxResources::LoadLinearSampler, dx)
{
}

Microsoft::WRL::ComPtr<ID3D11SamplerState> MediaRendererSamplerDxResources::GetPointSampler() {
	return this->pointSampler.Get();
}

Microsoft::WRL::ComPtr<ID3D11SamplerState> MediaRendererSamplerDxResources::GetLinearSampler() {
	return this->linearSampler.Get();
}

Microsoft::WRL::ComPtr<ID3D11SamplerState> MediaRendererSamplerDxResources::LoadPointSampler(const std::shared_ptr<GuardedDeviceResources> &dx) {
	HRESULT hr = S_OK;
	D3D11_SAMPLER_DESC samplerDesc;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	std::memset(samplerDesc.BorderColor, 0, sizeof samplerDesc.BorderColor);
	samplerDesc.MinLOD = -FLT_MAX;
	samplerDesc.MaxLOD = FLT_MAX;

	auto dxDevMt = dx->GetMultithread();
	auto d3dDev = dxDevMt.GetD3DDevice();
	hr = d3dDev->CreateSamplerState(&samplerDesc, sampler.GetAddressOf());
	H::System::ThrowIfFailed(hr);

	return sampler;
}

Microsoft::WRL::ComPtr<ID3D11SamplerState> MediaRendererSamplerDxResources::LoadLinearSampler(const std::shared_ptr<GuardedDeviceResources> &dx) {
	HRESULT hr = S_OK;
	D3D11_SAMPLER_DESC samplerDesc;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	std::memset(samplerDesc.BorderColor, 0, sizeof samplerDesc.BorderColor);
	samplerDesc.MinLOD = -FLT_MAX;
	samplerDesc.MaxLOD = FLT_MAX;

	auto dxDevMt = dx->GetMultithread();
	auto d3dDev = dxDevMt.GetD3DDevice();
	hr = d3dDev->CreateSamplerState(&samplerDesc, sampler.GetAddressOf());
	H::System::ThrowIfFailed(hr);

	return sampler;
}