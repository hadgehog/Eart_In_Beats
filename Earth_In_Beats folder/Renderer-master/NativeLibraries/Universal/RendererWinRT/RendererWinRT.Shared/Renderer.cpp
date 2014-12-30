#include "pch.h"
#include "Renderer.h"
#include "DX11\DisplayTexture\DX11SwapChainPanelDisplayTexture.h"

#include <memory>

using namespace DX11::DisplayTexture;

using namespace Platform;
using namespace Windows::ApplicationModel;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::Graphics::Display;
using namespace Windows::System::Threading;

namespace RendererWinRT{

	Renderer::Renderer(){
	}

	Renderer::~Renderer(){
		this->coreInput->Dispatcher->StopProcessEvents();
	}

	void Renderer::Initialize(SwapChainPanel ^panel){
		this->InitVisualEventHandlers(panel);
		this->InitInputEventHandlers(panel);
		this->InitAppEventHandlers();

		DisplayInformation ^currentDisplayInformation = DisplayInformation::GetForCurrentView();

		std::unique_ptr<DisplayTexture> dispTex = std::unique_ptr<DisplayTexture>(new SwapChainPanelDisplayTexture(panel, currentDisplayInformation));
	}

	void Renderer::InitVisualEventHandlers(SwapChainPanel ^panel){
		auto window = Windows::UI::Xaml::Window::Current->CoreWindow;
		auto currentDisplayInformation = DisplayInformation::GetForCurrentView();

		window->VisibilityChanged +=
			ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this, &Renderer::OnVisibilityChanged);

		currentDisplayInformation->DpiChanged +=
			ref new TypedEventHandler<DisplayInformation^, Object^>(this, &Renderer::OnDpiChanged);

		currentDisplayInformation->OrientationChanged +=
			ref new TypedEventHandler<DisplayInformation^, Object^>(this, &Renderer::OnOrientationChanged);

		DisplayInformation::DisplayContentsInvalidated +=
			ref new TypedEventHandler<DisplayInformation^, Object^>(this, &Renderer::OnDisplayContentsInvalidated);

		panel->CompositionScaleChanged +=
			ref new TypedEventHandler<SwapChainPanel^, Object^>(this, &Renderer::OnCompositionScaleChanged);

		panel->SizeChanged +=
			ref new SizeChangedEventHandler(this, &Renderer::OnSwapChainPanelSizeChanged);
	}

	void Renderer::InitInputEventHandlers(SwapChainPanel ^panel){
		// Disable all pointer visual feedback for better performance when touching.
		auto pointerVisualizationSettings = PointerVisualizationSettings::GetForCurrentView();
		pointerVisualizationSettings->IsContactFeedbackEnabled = false;
		pointerVisualizationSettings->IsBarrelButtonFeedbackEnabled = false;

		// Register our SwapChainPanel to get independent input pointer events
		auto workItemHandler = ref new WorkItemHandler([=](IAsyncAction ^)
		{
			// The CoreIndependentInputSource will raise pointer events for the specified device types on whichever thread it's created on.
			this->coreInput = panel->CreateCoreIndependentInputSource(
				Windows::UI::Core::CoreInputDeviceTypes::Mouse |
				Windows::UI::Core::CoreInputDeviceTypes::Touch |
				Windows::UI::Core::CoreInputDeviceTypes::Pen
				);

			// Register for pointer events, which will be raised on the background thread.
			this->coreInput->PointerPressed += ref new TypedEventHandler<Object^, PointerEventArgs^>(this, &Renderer::OnPointerPressed);
			this->coreInput->PointerMoved += ref new TypedEventHandler<Object^, PointerEventArgs^>(this, &Renderer::OnPointerMoved);
			this->coreInput->PointerReleased += ref new TypedEventHandler<Object^, PointerEventArgs^>(this, &Renderer::OnPointerReleased);

			// Begin processing input messages as they're delivered.
			this->coreInput->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessUntilQuit);
		});

		// Run task on a dedicated high priority background thread.
		this->inputLoopWorker = ThreadPool::RunAsync(workItemHandler, WorkItemPriority::High, WorkItemOptions::TimeSliced);
	}

	void Renderer::InitAppEventHandlers(){
		auto app = Windows::UI::Xaml::Application::Current;

		app->Suspending += ref new SuspendingEventHandler(this, &Renderer::OnSuspending);
		app->Resuming += ref new EventHandler<Object^>(this, &Renderer::OnResuming);
	}

	// Window event handlers.
	void Renderer::OnVisibilityChanged(CoreWindow^ sender, VisibilityChangedEventArgs^ args){

	}

	// Display event handlers.
	void Renderer::OnDpiChanged(DisplayInformation^ sender, Object^ args){

	}

	void Renderer::OnOrientationChanged(DisplayInformation^ sender, Object^ args){

	}

	void Renderer::OnCompositionScaleChanged(SwapChainPanel^ sender, Object^ args){

	}

	void Renderer::OnSwapChainPanelSizeChanged(Object^ sender, SizeChangedEventArgs^ e){

	}

	void Renderer::OnDisplayContentsInvalidated(DisplayInformation^ sender, Object^ args){

	}

	// Independent input handling functions.
	void Renderer::OnPointerPressed(Object^ sender, PointerEventArgs^ e){

	}

	void Renderer::OnPointerMoved(Object^ sender, PointerEventArgs^ e){

	}

	void Renderer::OnPointerReleased(Object^ sender, PointerEventArgs^ e){

	}

	// App events
	void Renderer::OnSuspending(Object^ sender, SuspendingEventArgs^ e){

	}

	void Renderer::OnResuming(Object ^sender, Object ^args){

	}

}