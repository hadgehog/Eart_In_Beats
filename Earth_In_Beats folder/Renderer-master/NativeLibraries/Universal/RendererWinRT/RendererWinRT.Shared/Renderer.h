#pragma once

namespace RendererWinRT{

	[Windows::Foundation::Metadata::WebHostHiddenAttribute]
	public ref class Renderer sealed{
	public:
		Renderer();
		virtual ~Renderer();

		void Initialize(Windows::UI::Xaml::Controls::SwapChainPanel ^panel);
	private:
		// Track our independent input on a background worker thread.
		Windows::Foundation::IAsyncAction^ inputLoopWorker;
		Windows::UI::Core::CoreIndependentInputSource^ coreInput;

		void InitVisualEventHandlers(Windows::UI::Xaml::Controls::SwapChainPanel ^panel);
		void InitInputEventHandlers(Windows::UI::Xaml::Controls::SwapChainPanel ^panel);
		void InitAppEventHandlers();

		// Window event handlers.
		void OnVisibilityChanged(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::VisibilityChangedEventArgs^ args);

		// Display event handlers.
		void OnDpiChanged(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args);
		void OnOrientationChanged(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args);
		void OnCompositionScaleChanged(Windows::UI::Xaml::Controls::SwapChainPanel^ sender, Platform::Object^ args);
		void OnSwapChainPanelSizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e);
		void OnDisplayContentsInvalidated(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args);

		// Independent input handling functions.
		void OnPointerPressed(Platform::Object^ sender, Windows::UI::Core::PointerEventArgs^ e);
		void OnPointerMoved(Platform::Object^ sender, Windows::UI::Core::PointerEventArgs^ e);
		void OnPointerReleased(Platform::Object^ sender, Windows::UI::Core::PointerEventArgs^ e);

		// App events
		void OnSuspending(Platform::Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ e);
		void OnResuming(Platform::Object ^sender, Platform::Object ^args);
	};

}