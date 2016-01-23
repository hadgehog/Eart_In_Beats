#include "Renderer.h"
#include "..\Helpers\H.h"

#include <mfidl.h>

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::UI::Input;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::Graphics::Display;
using namespace Windows::System::Threading;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::Storage::Streams;
using namespace Concurrency;

namespace EarthInBeatsNativeLibrary
{
	Renderer::Renderer()
		: pointerMoves(false){
	}

	Renderer::~Renderer(){
		this->coreInput->Dispatcher->StopProcessEvents();

		// wait for input loop to complete
		H::System::PerformSync(this->inputLoopWorker);

		this->window->VisibilityChanged -= this->visibilityChangedEventToken;
		this->displayInfo->DpiChanged -= this->dpiChangedEventToken;
		this->displayInfo->OrientationChanged -= this->orientationChangedEventToken;
		DisplayInformation::DisplayContentsInvalidated -= this->displayInvalidatedEventToken;
		this->panel->CompositionScaleChanged -= this->compositionScaleChangedEventToken;
		this->panel->SizeChanged -= this->sizeChangedEventToken;
	}

	Windows::UI::Color Renderer::BackgroundColor::get(){
		Windows::UI::Color color;
		auto tmpColor = this->main.GetClearColor();

		color.R = static_cast<uint8_t>(tmpColor[0] * 255.0f);
		color.G = static_cast<uint8_t>(tmpColor[1] * 255.0f);
		color.B = static_cast<uint8_t>(tmpColor[2] * 255.0f);
		color.A = static_cast<uint8_t>(tmpColor[3] * 255.0f);

		return color;
	}

	void Renderer::BackgroundColor::set(Windows::UI::Color v){
		const float scale = 1.0f / 255.0f;
		this->main.SetClearColor(
			static_cast<float>(v.R) * scale, 
			static_cast<float>(v.G) * scale, 
			static_cast<float>(v.B) * scale, 
			static_cast<float>(v.A) * scale);
	}

	void Renderer::Initialize(CoreWindow ^window, SwapChainPanel ^panel, IRenderable ^renderable){
		this->InitEventHandlers(window, panel);
		this->InitInput(panel);

		this->main.Initialize(panel, renderable->Get());
	}

	void Renderer::SaveState(){
		this->main.SaveState();
	}

	void Renderer::LoadState(){
		this->main.LoadState();
	}

	void Renderer::InitEventHandlers(CoreWindow ^window, SwapChainPanel ^panel){
		DisplayInformation^ currentDisplayInformation = DisplayInformation::GetForCurrentView();

		this->window = window;
		this->panel = panel;
		this->displayInfo = currentDisplayInformation;

		this->visibilityChangedEventToken = window->VisibilityChanged +=
			ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this, &Renderer::OnVisibilityChanged);

		this->dpiChangedEventToken = currentDisplayInformation->DpiChanged +=
			ref new TypedEventHandler<DisplayInformation^, Object^>(this, &Renderer::OnDpiChanged);

		this->orientationChangedEventToken = currentDisplayInformation->OrientationChanged +=
			ref new TypedEventHandler<DisplayInformation^, Object^>(this, &Renderer::OnOrientationChanged);

		this->displayInvalidatedEventToken = DisplayInformation::DisplayContentsInvalidated +=
			ref new TypedEventHandler<DisplayInformation^, Object^>(this, &Renderer::OnDisplayContentsInvalidated);

		this->compositionScaleChangedEventToken = panel->CompositionScaleChanged +=
			ref new TypedEventHandler<SwapChainPanel^, Object^>(this, &Renderer::OnCompositionScaleChanged);

		this->sizeChangedEventToken = panel->SizeChanged +=
			ref new SizeChangedEventHandler(this, &Renderer::OnSwapChainPanelSizeChanged);
	}

	void Renderer::InitInput(SwapChainPanel ^panel){
		// Disable all pointer visual feedback for better performance when touching.
		/*auto pointerVisualizationSettings = PointerVisualizationSettings::GetForCurrentView();
		pointerVisualizationSettings->IsContactFeedbackEnabled = false;
		pointerVisualizationSettings->IsBarrelButtonFeedbackEnabled = false;*/

		// Register our SwapChainPanel to get independent input pointer events
		auto workItemHandler = ref new WorkItemHandler([this, panel](IAsyncAction ^)
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
			this->coreInput->PointerWheelChanged += ref new TypedEventHandler<Object^, PointerEventArgs^>(this, &Renderer::OnPointerWheelChanged);

			// Begin processing input messages as they're delivered.
			this->coreInput->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessUntilQuit);
		});

		// Run task on a dedicated high priority background thread.
		this->inputLoopWorker = ThreadPool::RunAsync(workItemHandler, WorkItemPriority::High, WorkItemOptions::TimeSliced);
	}

	void Renderer::OnVisibilityChanged(CoreWindow ^sender, VisibilityChangedEventArgs ^args){
		this->main.VisibilityChanged(args->Visible);
	}

	void Renderer::OnDpiChanged(DisplayInformation^ sender, Object^ args){
		this->main.DpiChanged(sender->LogicalDpi);
	}

	void Renderer::OnOrientationChanged(DisplayInformation^ sender, Object^ args){
		this->main.OrientationChanged(sender->CurrentOrientation);
	}

	void Renderer::OnDisplayContentsInvalidated(DisplayInformation^ sender, Object^ args){
		this->main.DisplayContentsInvalidated();
	}

	void Renderer::OnCompositionScaleChanged(SwapChainPanel^ sender, Object^ args){
		this->main.CompositionScaleChanged(sender->CompositionScaleX, sender->CompositionScaleY);
	}

	void Renderer::OnSwapChainPanelSizeChanged(Object^ sender, SizeChangedEventArgs^ e){
		this->main.SizeChanged(e->NewSize);
	}

	void Renderer::OnPointerPressed(Object^ sender, PointerEventArgs^ e){
		auto pt = e->CurrentPoint;
		this->pointerMoves = true;
		
		this->main.PointerPressed(pt);
	}

	void Renderer::OnPointerMoved(Object^ sender, PointerEventArgs^ e){
		if (this->pointerMoves){
			auto pt = e->CurrentPoint;

			this->main.PointerMoved(pt);
		}
	}

	void Renderer::OnPointerReleased(Object^ sender, PointerEventArgs^ e){
		auto pt = e->CurrentPoint;
		this->pointerMoves = false;

		this->main.PointerReleased(pt);
	}
	void Renderer::OnPointerWheelChanged(Platform::Object ^sender, Windows::UI::Core::PointerEventArgs ^e){
		auto pt = e->CurrentPoint;

		this->main.PointerWheelChanged(pt);
	}
}