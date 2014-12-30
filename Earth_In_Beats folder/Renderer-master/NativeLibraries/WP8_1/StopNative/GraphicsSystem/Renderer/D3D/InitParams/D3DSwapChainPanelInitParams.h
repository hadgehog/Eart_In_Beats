#pragma once

#include <string>

class D3DSwapChainPanelInitParams
{
public:

	static const std::string ParameterKey;

	D3DSwapChainPanelInitParams();

	~D3DSwapChainPanelInitParams();

	Windows::UI::Xaml::Controls::SwapChainPanel ^GetPanel() const;
	void SetPanel(Windows::UI::Xaml::Controls::SwapChainPanel ^v);

private:

	Windows::UI::Xaml::Controls::SwapChainPanel ^panel;
};