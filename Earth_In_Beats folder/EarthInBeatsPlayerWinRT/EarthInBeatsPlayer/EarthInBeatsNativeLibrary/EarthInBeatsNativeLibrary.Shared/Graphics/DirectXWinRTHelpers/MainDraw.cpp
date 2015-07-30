#include "MainDraw.h"
#include "..\Helpers\H.h"

#include <fstream>

using namespace Concurrency;
using namespace Windows::Foundation;
using namespace Windows::System::Threading;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Input;

MainDraw::MainDraw()
{
	this->clearColor.fill(0.0f);
}

MainDraw::~MainDraw()
{
	this->StopRenderLoop();

	{
		// ... and wait while it will release its lock(i.e. when it will exit)
		// H::System::PerformSync CAN'T be use here because it exits when task is cancelled
		// but cancelled task is NOT finished task(i.e. thread may stay alive for some time after task is cancelled).
		critical_section::scoped_lock lock(this->renderLoopCS);
	}

	auto dxDev = this->dx->Get();
	dxDev->RegisterDeviceNotify(nullptr);
}

std::array<float, 4> MainDraw::GetClearColor(){
	critical_section::scoped_lock lock(this->criticalSection);
	return this->clearColor;
}

void MainDraw::SetClearColor(const std::array<float, 4> &v){
	critical_section::scoped_lock lock(this->criticalSection);
	this->clearColor = v;
}

void MainDraw::SetClearColor(float r, float g, float b, float a){
	critical_section::scoped_lock lock(this->criticalSection);
	this->clearColor[0] = r;
	this->clearColor[1] = g;
	this->clearColor[2] = b;
	this->clearColor[3] = a;
}

void MainDraw::SetClearColor(float r, float g, float b){
	critical_section::scoped_lock lock(this->criticalSection);
	this->clearColor[0] = r;
	this->clearColor[1] = g;
	this->clearColor[2] = b;
	this->clearColor[3] = 1.0f;
}

void MainDraw::Initialize(SwapChainPanel ^panel, EarthInBeatsNativeLibrary::NativeRenderableContainer ^container){
	this->dx = std::make_shared<GuardedDeviceResources>();

	{
		auto dxDev = this->dx->Get();
		dxDev->SetSwapChainPanel(panel);
		dxDev->RegisterDeviceNotify(this);
	}

	this->nativeRenderer = container->Get();
	this->nativeRenderer->Initialize(this->dx);
	this->nativeRenderer->CreateDeviceDependentResources();

	if (panel->ActualWidth > 0 && panel->ActualHeight > 0){
		this->nativeRenderer->CreateSizeDependentResources();
	}

	this->StartRenderLoop();
}

void MainDraw::SaveState(){
	critical_section::scoped_lock lock(this->criticalSection);
	
	{
		auto dxDev = this->dx->Get();
		dxDev->Trim();
	}

	// Stop rendering when the app is suspended.
	this->StopRenderLoop();
}

void MainDraw::LoadState(){
	this->StartRenderLoop();
}

void MainDraw::VisibilityChanged(bool visible){
	if (visible){
		this->StartRenderLoop();
	}
	else{
		this->StopRenderLoop();
	}
}

void MainDraw::DpiChanged(float dpi){
	critical_section::scoped_lock lock(this->criticalSection);
	
	{
		auto dxDev = this->dx->Get();
		dxDev->SetDpi(dpi);
	}

	this->CreateSizeDependentResources();
}

void MainDraw::OrientationChanged(Windows::Graphics::Display::DisplayOrientations orientation){
	critical_section::scoped_lock lock(this->criticalSection);
	
	{
		auto dxDev = this->dx->Get();
		dxDev->SetCurrentOrientation(orientation);
	}

	this->CreateSizeDependentResources();
}

void MainDraw::DisplayContentsInvalidated(){
	critical_section::scoped_lock lock(this->criticalSection);
	auto dxDev = this->dx->Get();
	dxDev->ValidateDevice();
}

void MainDraw::CompositionScaleChanged(float scaleX, float scaleY){
	critical_section::scoped_lock lock(this->criticalSection);
	
	{
		auto dxDev = this->dx->Get();
		dxDev->SetCompositionScale(scaleX, scaleY);
	}

	this->CreateSizeDependentResources();
}

void MainDraw::SizeChanged(Windows::Foundation::Size size){
	critical_section::scoped_lock lock(this->criticalSection);
	
	{
		auto dxDev = this->dx->Get();
		dxDev->SetLogicalSize(size);
	}

	this->CreateSizeDependentResources();
}

void MainDraw::PointerPressed(PointerPoint ^ppt){
	this->nativeRenderer->PointerPressed(ppt);
}

void MainDraw::PointerMoved(PointerPoint ^ppt){
	this->nativeRenderer->PointerMoved(ppt);
}

void MainDraw::PointerReleased(PointerPoint ^ppt){
	this->nativeRenderer->PointerReleased(ppt);
}

void MainDraw::OnDeviceLost(){
	this->ReleaseDeviceDependentResources();
}

void MainDraw::OnDeviceRestored(){
	this->CreateDeviceDependentResources();
	this->CreateSizeDependentResources();
}

void MainDraw::CreateDeviceDependentResources(){
	this->nativeRenderer->CreateDeviceDependentResources();
}

void MainDraw::ReleaseDeviceDependentResources(){
	this->nativeRenderer->ReleaseDeviceDependentResources();
}

void MainDraw::CreateSizeDependentResources(){
	this->nativeRenderer->CreateSizeDependentResources();
}

void MainDraw::StartRenderLoop(){
	// If the animation render loop is already running then do not start another thread.
	if (this->renderLoopWorker != nullptr && this->renderLoopWorker->Status == AsyncStatus::Started){
		return;
	}

	// Create a task that will be run on a background thread.
	auto workItemHandler = ref new WorkItemHandler([this](IAsyncAction ^action){
		critical_section::scoped_lock lock(this->renderLoopCS);

		this->nativeRenderer->OnRenderThreadStart();

		// Calculate the updated frame and render once per vertical blanking interval.
		bool working = true;
		while (action->Status == AsyncStatus::Started){
			critical_section::scoped_lock lock(this->criticalSection);
			this->Update();
			if (this->Render()){
				auto dxDev = this->dx->Get();
				dxDev->Present();
			}
		}

		this->nativeRenderer->OnRenderThreadEnd();
	});

	// Run task on a dedicated high priority background thread.
	this->renderLoopWorker = ThreadPool::RunAsync(workItemHandler, WorkItemPriority::High, WorkItemOptions::TimeSliced);
}

void MainDraw::StopRenderLoop(){
	this->renderLoopWorker->Cancel();
}

void MainDraw::ProcessInput(){

}

void MainDraw::Update(){
	this->timer.Tick([&](){
		this->nativeRenderer->Update(this->timer);
	});
}

bool MainDraw::Render(){
	// Don't try to render anything before the first Update.
	if (this->timer.GetFrameCount() == 0){
		return false;
	}

	{
		auto dxDev = this->dx->Get();
		auto context = dxDev->GetD3DDeviceContext();

		// Reset the viewport to target the whole screen.
		auto viewport = dxDev->GetScreenViewport();
		context->RSSetViewports(1, &viewport);

		// Reset render targets to the screen.
		ID3D11RenderTargetView *const targets[1] = { dxDev->GetBackBufferRenderTargetView() };
		context->OMSetRenderTargets(1, targets, dxDev->GetDepthStencilView());

		// Clear the back buffer and depth stencil view.
		context->ClearRenderTargetView(dxDev->GetBackBufferRenderTargetView(), this->clearColor.data());
		context->ClearDepthStencilView(dxDev->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	this->nativeRenderer->Render();

	return true;
}
