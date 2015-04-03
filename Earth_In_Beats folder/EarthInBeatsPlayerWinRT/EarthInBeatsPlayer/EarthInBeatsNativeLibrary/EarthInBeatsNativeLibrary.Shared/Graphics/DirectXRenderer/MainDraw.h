#pragma once
#include "DirectXCommon\DeviceResources.h"
#include "INativeRenderable.h"
#include "NativeRenderableContainer.h"

#include <memory>
#include <future>
#include <array>

class MainDraw : public DX::IDeviceNotify{
public:
	MainDraw();
	virtual ~MainDraw();

	std::array<float, 4> GetClearColor();
	void SetClearColor(const std::array<float, 4> &v);
	void SetClearColor(float r, float g, float b, float a);
	void SetClearColor(float r, float g, float b);

	void Initialize(Windows::UI::Xaml::Controls::SwapChainPanel ^panel, EarthInBeatsNativeLibrary::NativeRenderableContainer ^container);

	void SaveState();
	void LoadState();

	void VisibilityChanged(bool visible);
	void DpiChanged(float dpi);
	void OrientationChanged(Windows::Graphics::Display::DisplayOrientations orientation);
	void DisplayContentsInvalidated();
	void CompositionScaleChanged(float scaleX, float scaleY);
	void SizeChanged(Windows::Foundation::Size size);

	void PointerPressed(Windows::UI::Input::PointerPoint ^ppt);
	void PointerMoved(Windows::UI::Input::PointerPoint ^ppt);
	void PointerReleased(Windows::UI::Input::PointerPoint ^ppt);

private:
	std::shared_ptr<INativeRenderable> nativeRenderer;

	DX::StepTimer timer;
	std::shared_ptr<DX::DeviceResources> dxDev;
	Windows::Foundation::IAsyncAction^ renderLoopWorker;
	Concurrency::critical_section renderLoopCS;
	std::array<float, 4> clearColor;

	Concurrency::critical_section criticalSection;

	// IDeviceNotify
	virtual void OnDeviceLost() override;
	virtual void OnDeviceRestored() override;

	void CreateDeviceDependentResources();
	void ReleaseDeviceDependentResources();

	void CreateSizeDependentResources();

	void StartRenderLoop();
	void StopRenderLoop();

	void ProcessInput();
	void Update();
	bool Render();
};