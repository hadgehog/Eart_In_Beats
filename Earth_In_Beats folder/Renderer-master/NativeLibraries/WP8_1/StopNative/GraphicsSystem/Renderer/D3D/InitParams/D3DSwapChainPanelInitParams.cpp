#include "pch.h"
#include "D3DSwapChainPanelInitParams.h"

using namespace Windows::UI::Xaml::Controls;

const std::string D3DSwapChainPanelInitParams::ParameterKey = "D3DSwapChainPanelInitParams";

D3DSwapChainPanelInitParams::D3DSwapChainPanelInitParams()
{
}

D3DSwapChainPanelInitParams::~D3DSwapChainPanelInitParams()
{
}

SwapChainPanel ^D3DSwapChainPanelInitParams::GetPanel() const
{
	return this->panel;
}

void D3DSwapChainPanelInitParams::SetPanel(SwapChainPanel ^v)
{
	this->panel = v;
}