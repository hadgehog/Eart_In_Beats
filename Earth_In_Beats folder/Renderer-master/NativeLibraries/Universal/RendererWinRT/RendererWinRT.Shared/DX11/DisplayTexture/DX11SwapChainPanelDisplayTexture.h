#pragma once
#include "DX11DisplayTexture.h"

#include <dxgi1_3.h>

namespace DX11{
	namespace DisplayTexture{

		class SwapChainPanelDisplayTexture : public DisplayTexture{
		public:
			SwapChainPanelDisplayTexture(
				Windows::UI::Xaml::Controls::SwapChainPanel ^swapChainPanel, 
				Windows::Graphics::Display::DisplayInformation ^currentDisplayInformation);
			virtual ~SwapChainPanelDisplayTexture();

			static void ConvOrientations(Windows::Graphics::Display::DisplayOrientations from, Orientation &to);
		private:
			// Cached reference to the XAML panel.
			Windows::UI::Xaml::Controls::SwapChainPanel ^swapChainPanel;
			Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain;
		};

	}
}