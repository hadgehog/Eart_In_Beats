#pragma once
#include "MainDraw.h"
#include "IRenderable.h"

namespace EarthInBeatsNativeLibrary
{
	public ref class Renderer sealed{
	public:
		Renderer();
		virtual ~Renderer();

		property Windows::UI::Color BackgroundColor{
			Windows::UI::Color get();
			void set(Windows::UI::Color v);
		}

		void Initialize(Windows::UI::Core::CoreWindow ^window, Windows::UI::Xaml::Controls::SwapChainPanel ^panel, IRenderable ^renderable);

		void SaveState();
		void LoadState();

	private:
		Main main;

		bool pointerMoves;

		// Track our independent input on a background worker thread.
		Windows::Foundation::IAsyncAction ^inputLoopWorker;
		Windows::UI::Core::CoreIndependentInputSource ^coreInput;

		Platform::Agile<Windows::UI::Core::CoreWindow> window;
		Windows::UI::Xaml::Controls::SwapChainPanel ^panel;
		Windows::Graphics::Display::DisplayInformation ^displayInfo;

		Windows::Foundation::EventRegistrationToken visibilityChangedEventToken;
		Windows::Foundation::EventRegistrationToken dpiChangedEventToken;
		Windows::Foundation::EventRegistrationToken orientationChangedEventToken;
		Windows::Foundation::EventRegistrationToken displayInvalidatedEventToken;
		Windows::Foundation::EventRegistrationToken compositionScaleChangedEventToken;
		Windows::Foundation::EventRegistrationToken sizeChangedEventToken;

		void InitEventHandlers(
			Windows::UI::Core::CoreWindow ^window,
			Windows::UI::Xaml::Controls::SwapChainPanel ^panel);

		void InitInput(Windows::UI::Xaml::Controls::SwapChainPanel ^panel);

		void OnVisibilityChanged(Windows::UI::Core::CoreWindow ^sender, Windows::UI::Core::VisibilityChangedEventArgs ^args);
		void OnDpiChanged(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args);
		void OnOrientationChanged(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args);
		void OnDisplayContentsInvalidated(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args);
		void OnCompositionScaleChanged(Windows::UI::Xaml::Controls::SwapChainPanel^ sender, Platform::Object^ args);
		void OnSwapChainPanelSizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e);

		// Independent input handling functions.
		void OnPointerPressed(Platform::Object^ sender, Windows::UI::Core::PointerEventArgs^ e);
		void OnPointerMoved(Platform::Object^ sender, Windows::UI::Core::PointerEventArgs^ e);
		void OnPointerReleased(Platform::Object^ sender, Windows::UI::Core::PointerEventArgs^ e);
	};
}