#pragma once

#include "..\..\DirectXWinRTHelpers\INativeRenderable.h"
#include "Graphics\DirectXRenderer\Helpers\Timer.h"

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

	virtual void Initialize(const std::shared_ptr<DX::DeviceResources> &dxDev) override;

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

private:
	std::shared_ptr<DX::DeviceResources> dxDev;

	//std::unique_ptr<Quad> fsQuad;
	DirectX::XMFLOAT4X4 fsQuadBaseLocalTransform;
	DirectX::XMFLOAT4X4 fsQuadBaseWorldTranslation;
	DirectX::XMFLOAT4X4 projection;


};

