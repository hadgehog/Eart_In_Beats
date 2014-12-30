#pragma once

#include <D3D11_2.h>
#include <d2d1_2.h>
#include <dwrite_2.h>
#include <wincodec.h>
#include <directxmath.h>
#include <wrl.h>

class RendererD3DServiceData{
public:
	RendererD3DServiceData();
	~RendererD3DServiceData();

	const Microsoft::WRL::ComPtr<ID3D11Device2> &GetD3DDevice() const;
	void SetD3DDevice(Microsoft::WRL::ComPtr<ID3D11Device2> &v);

	const Microsoft::WRL::ComPtr<ID3D11DeviceContext2> &GetD3DCtx() const;
	void SetD3DCtx(Microsoft::WRL::ComPtr<ID3D11DeviceContext2> &v);

	const Microsoft::WRL::ComPtr<IDXGISwapChain1> &GetSwapChain() const;
	void SetSwapChain(Microsoft::WRL::ComPtr<IDXGISwapChain1> &v);

	const Microsoft::WRL::ComPtr<ID3D11RenderTargetView> &GetD3DRT() const;
	void SetD3DRT(Microsoft::WRL::ComPtr<ID3D11RenderTargetView> &v);

	const Microsoft::WRL::ComPtr<ID3D11DepthStencilView> &GetDepthStencil() const;
	void SetDepthStencil(Microsoft::WRL::ComPtr<ID3D11DepthStencilView> &v);

	const D3D11_VIEWPORT &GetScreenViewport() const;
	void SetScreenViewport(D3D11_VIEWPORT &v);

	const Microsoft::WRL::ComPtr<ID2D1Factory2> &GetD2DFactory() const;
	void SetD2DFactory(Microsoft::WRL::ComPtr<ID2D1Factory2> &v);

	const Microsoft::WRL::ComPtr<ID2D1Device1> &GetD2DDevice() const;
	void SetD2DDevice(Microsoft::WRL::ComPtr<ID2D1Device1> &v);

	const Microsoft::WRL::ComPtr<ID2D1DeviceContext1> &GetD2DCtx() const;
	void SetD2DCtx(Microsoft::WRL::ComPtr<ID2D1DeviceContext1> &v);

	const Microsoft::WRL::ComPtr<ID2D1Bitmap1> &GetD2DRT() const;
	void SetD2DRT(Microsoft::WRL::ComPtr<ID2D1Bitmap1> &v);

	const Microsoft::WRL::ComPtr<IDWriteFactory2> &GetDWriteFactory() const;
	void SetDWriteFactory(Microsoft::WRL::ComPtr<IDWriteFactory2> &v);

	const Microsoft::WRL::ComPtr<IWICImagingFactory2> &GetWICFactory() const;
	void SetWICFactory(Microsoft::WRL::ComPtr<IWICImagingFactory2> &v);

	Windows::UI::Xaml::Controls::SwapChainPanel ^GetSwapChainPanel() const;
	void SetSwapChainPanel(Windows::UI::Xaml::Controls::SwapChainPanel ^v);

	D3D_FEATURE_LEVEL GetFeatureLevel() const;
	void SetFeatureLevel(D3D_FEATURE_LEVEL v);

	Windows::Foundation::Size GetRTSize() const;
	void SetRTSize(Windows::Foundation::Size v);

	Windows::Foundation::Size GetOutputSize() const;
	void SetOutputSize(Windows::Foundation::Size v);

	Windows::Foundation::Size GetLogicalSize() const;
	void SetLogicalSize(Windows::Foundation::Size v);

	Windows::Graphics::Display::DisplayOrientations GetNativeOrientation() const;
	void SetNativeOrientation(Windows::Graphics::Display::DisplayOrientations v);

	Windows::Graphics::Display::DisplayOrientations GetCurrentOrientation() const;
	void SetCurrentOrientation(Windows::Graphics::Display::DisplayOrientations v);

	float GetDPI() const;
	void SetDPI(float v);

	DirectX::XMFLOAT2 GetCompositionScale() const;
	void SetCompositionScale(DirectX::XMFLOAT2 v);

	const D2D1::Matrix3x2F &GetOrientationTransform2D() const;
	void SetOrientationTransform2D(D2D1::Matrix3x2F &v);

	const DirectX::XMFLOAT4X4 &GetOrientationTransform3D() const;
	void SetOrientationTransform3D(DirectX::XMFLOAT4X4 &v);

private:

	Microsoft::WRL::ComPtr<ID3D11Device2> d3dDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext2> d3dContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> d3dRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> d3dDepthStencilView;
	D3D11_VIEWPORT screenViewport;

	Microsoft::WRL::ComPtr<ID2D1Factory2> d2dFactory;
	Microsoft::WRL::ComPtr<ID2D1Device1> d2dDevice;
	Microsoft::WRL::ComPtr<ID2D1DeviceContext1> d2dContext;
	Microsoft::WRL::ComPtr<ID2D1Bitmap1> d2dTargetBitmap;

	Microsoft::WRL::ComPtr<IDWriteFactory2> dwriteFactory;
	Microsoft::WRL::ComPtr<IWICImagingFactory2> wicFactory;

	// Cached reference to the XAML panel.
	Windows::UI::Xaml::Controls::SwapChainPanel^ swapChainPanel;

	// Cached device properties.
	D3D_FEATURE_LEVEL d3dFeatureLevel;
	Windows::Foundation::Size d3dRenderTargetSize;
	Windows::Foundation::Size outputSize;
	Windows::Foundation::Size logicalSize;
	Windows::Graphics::Display::DisplayOrientations nativeOrientation;
	Windows::Graphics::Display::DisplayOrientations currentOrientation;
	float dpi;
	DirectX::XMFLOAT2 compositionScale;

	// Transforms used for display orientation.
	D2D1::Matrix3x2F orientationTransform2D;
	DirectX::XMFLOAT4X4 orientationTransform3D;
};