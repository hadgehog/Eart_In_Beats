#pragma once

#include "GraphicsSystem\Renderer\RendererProxy.h"

namespace StopNative
{
	[Windows::Foundation::Metadata::WebHostHiddenAttribute]
	public ref class D3DView sealed
	{
	public:

		D3DView();

		void Initialize(Windows::UI::Xaml::Controls::SwapChainPanel ^panel);

	private:

		RendererProxy rendererProxy;

	};
}