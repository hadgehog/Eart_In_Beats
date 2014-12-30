#include "pch.h"
#include "D3DView.h"
#include "GraphicsSystem\Renderer\D3D\InitParams\D3DSwapChainPanelInitParams.h"

namespace StopNative
{
	D3DView::D3DView()
	{
	}

	void D3DView::Initialize(Windows::UI::Xaml::Controls::SwapChainPanel ^panel)
	{
		std::map<std::string, std::shared_ptr<void>> initParams;
		D3DSwapChainPanelInitParams *params = new D3DSwapChainPanelInitParams();

		params->SetPanel(panel);

		initParams.insert(std::make_pair(D3DSwapChainPanelInitParams::ParameterKey, std::shared_ptr<void>(params)));

		this->rendererProxy.Initialize(initParams);
	}
}