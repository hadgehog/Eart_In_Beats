#pragma once
#include "AsyncResource.h"
#include "..\DirectXCommon\GuardedDeviceResources.h"

class MediaRendererSamplerDxResources {
public:
	MediaRendererSamplerDxResources(const std::shared_ptr<GuardedDeviceResources> &dx);

	Microsoft::WRL::ComPtr<ID3D11SamplerState> GetPointSampler();
	Microsoft::WRL::ComPtr<ID3D11SamplerState> GetLinearSampler();

private:
	AsyncResource<Microsoft::WRL::ComPtr<ID3D11SamplerState>> pointSampler;
	AsyncResource<Microsoft::WRL::ComPtr<ID3D11SamplerState>> linearSampler;

	static Microsoft::WRL::ComPtr<ID3D11SamplerState> LoadPointSampler(const std::shared_ptr<GuardedDeviceResources> &dx);
	static Microsoft::WRL::ComPtr<ID3D11SamplerState> LoadLinearSampler(const std::shared_ptr<GuardedDeviceResources> &dx);
};