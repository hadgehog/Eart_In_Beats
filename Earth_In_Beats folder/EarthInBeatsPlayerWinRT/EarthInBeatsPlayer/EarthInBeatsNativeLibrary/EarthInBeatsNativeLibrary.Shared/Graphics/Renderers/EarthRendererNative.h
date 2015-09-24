#pragma once

#include "..\DirectXWinRTHelpers\INativeRenderable.h"
#include "Graphics\Helpers\Timer.h"
#include "VertexTextureNormal.h"
#include "Graphics\Helpers\Thread\PPL\safe_task.h"
#include "..\DirectXResources\MediaRendererDxResources.h"
#include "..\Shaders\QuadVertexShaderSettings.h"

#include <memory>
#include <string>
#include <vector>
#include <cstdint>
#include <condition_variable>
#include <mutex>
#include <stdlib.h>

class EarthRendererNative : public INativeRenderable{
public:
	EarthRendererNative();
	~EarthRendererNative();

	virtual void Initialize(const std::shared_ptr<GuardedDeviceResources> &dx) override;

	virtual void CreateDeviceDependentResources() override;
	virtual void ReleaseDeviceDependentResources() override;

	virtual void CreateSizeDependentResources() override;

	virtual void OnRenderThreadStart() override;
	virtual void OnRenderThreadEnd() override;

	virtual void Update(const DX::StepTimer &timer) override;
	virtual void Render() override;

	virtual void PointerPressed(Windows::UI::Input::PointerPoint ^ppt) override;
	virtual void PointerMoved(Windows::UI::Input::PointerPoint ^ppt) override;
	virtual void PointerReleased(Windows::UI::Input::PointerPoint ^ppt) override;

	void LoadModel(std::string path);

private:
	std::shared_ptr<GuardedDeviceResources> dx;
	DirectX::XMFLOAT4X4 projection;
	std::unique_ptr<MediaRendererDxResources> dxResources;

	concurrency::critical_section dataCs;

	concurrency::safe_task<void> initializeTask;
	std::mutex initializedMtx;
	std::condition_variable inititalizedCv;
	bool initialized;

	std::shared_ptr<Geometry> quad;
	std::shared_ptr<QuadVs> quadVs;
	std::unique_ptr<QuadVs::Cbuffer> quadVsCbuffer;
	std::shared_ptr<QuadPs> quadPs;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> quadSampler;

	std::vector<VertexTextureNormal> posTexNormList;

	void WaitForInitialization();
};

