#pragma once
#include "DirectXCommon\DeviceResources.h"
#include "DirectXCommon\StepTimer.h"

#include <memory>

class INativeRenderable
{
public:
	INativeRenderable();
	virtual ~INativeRenderable();

	virtual void Initialize(const std::shared_ptr<DX::DeviceResources> &dxDev) = 0;

	virtual void CreateDeviceDependentResources() = 0;
	virtual void ReleaseDeviceDependentResources() = 0;

	virtual void CreateSizeDependentResources() = 0;

	virtual void OnRenderThreadStart() = 0;
	virtual void OnRenderThreadEnd() = 0;

	virtual void Update(const DX::StepTimer &timer) = 0;
	virtual void Render() = 0;

	virtual void PointerPressed(Windows::UI::Input::PointerPoint ^ppt) = 0;
	virtual void PointerMoved(Windows::UI::Input::PointerPoint ^ppt) = 0;
	virtual void PointerReleased(Windows::UI::Input::PointerPoint ^ppt) = 0;

};

