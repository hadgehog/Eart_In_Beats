#include "pch.h"
#include "RendererD3DServiceData.h"

RendererD3DServiceData::RendererD3DServiceData(){
}

RendererD3DServiceData::~RendererD3DServiceData(){
}

const Microsoft::WRL::ComPtr<ID3D11Device2> &RendererD3DServiceData::GetD3DDevice() const{
	return this->d3dDevice;
}

void RendererD3DServiceData::SetD3DDevice(Microsoft::WRL::ComPtr<ID3D11Device2> &v){
	this->d3dDevice = v;
}

const Microsoft::WRL::ComPtr<ID3D11DeviceContext2> &RendererD3DServiceData::GetD3DCtx() const{
	return this->d3dContext;
}

void RendererD3DServiceData::SetD3DCtx(Microsoft::WRL::ComPtr<ID3D11DeviceContext2> &v){
	this->d3dContext = v;
}

const Microsoft::WRL::ComPtr<IDXGISwapChain1> &RendererD3DServiceData::GetSwapChain() const{
	return this->swapChain;
}

void RendererD3DServiceData::SetSwapChain(Microsoft::WRL::ComPtr<IDXGISwapChain1> &v){
	this->swapChain = v;
}

const Microsoft::WRL::ComPtr<ID3D11RenderTargetView> &RendererD3DServiceData::GetD3DRT() const{
	return this->d3dRenderTargetView;
}

void RendererD3DServiceData::SetD3DRT(Microsoft::WRL::ComPtr<ID3D11RenderTargetView> &v){
	this->d3dRenderTargetView = v;
}

const Microsoft::WRL::ComPtr<ID3D11DepthStencilView> &RendererD3DServiceData::GetDepthStencil() const{
	return this->d3dDepthStencilView;
}

void RendererD3DServiceData::SetDepthStencil(Microsoft::WRL::ComPtr<ID3D11DepthStencilView> &v){
	this->d3dDepthStencilView = v;
}

const D3D11_VIEWPORT &RendererD3DServiceData::GetScreenViewport() const{
	return this->screenViewport;
}

void RendererD3DServiceData::SetScreenViewport(D3D11_VIEWPORT &v){
	this->screenViewport = v;
}

const Microsoft::WRL::ComPtr<ID2D1Factory2> &RendererD3DServiceData::GetD2DFactory() const{
	return this->d2dFactory;
}

void RendererD3DServiceData::SetD2DFactory(Microsoft::WRL::ComPtr<ID2D1Factory2> &v){
	this->d2dFactory = v;
}

const Microsoft::WRL::ComPtr<ID2D1Device1> &RendererD3DServiceData::GetD2DDevice() const{
	return this->d2dDevice;
}

void RendererD3DServiceData::SetD2DDevice(Microsoft::WRL::ComPtr<ID2D1Device1> &v){
	this->d2dDevice = v;
}

const Microsoft::WRL::ComPtr<ID2D1DeviceContext1> &RendererD3DServiceData::GetD2DCtx() const{
	return this->d2dContext;
}

void RendererD3DServiceData::SetD2DCtx(Microsoft::WRL::ComPtr<ID2D1DeviceContext1> &v){
	this->d2dContext = v;
}

const Microsoft::WRL::ComPtr<ID2D1Bitmap1> &RendererD3DServiceData::GetD2DRT() const{
	return this->d2dTargetBitmap;
}

void RendererD3DServiceData::SetD2DRT(Microsoft::WRL::ComPtr<ID2D1Bitmap1> &v){
	this->d2dTargetBitmap = v;
}

const Microsoft::WRL::ComPtr<IDWriteFactory2> &RendererD3DServiceData::GetDWriteFactory() const{
	return this->dwriteFactory;
}

void RendererD3DServiceData::SetDWriteFactory(Microsoft::WRL::ComPtr<IDWriteFactory2> &v){
	this->dwriteFactory = v;
}

const Microsoft::WRL::ComPtr<IWICImagingFactory2> &RendererD3DServiceData::GetWICFactory() const{
	return this->wicFactory;
}

void RendererD3DServiceData::SetWICFactory(Microsoft::WRL::ComPtr<IWICImagingFactory2> &v){
	this->wicFactory = v;
}

Windows::UI::Xaml::Controls::SwapChainPanel ^RendererD3DServiceData::GetSwapChainPanel() const{
	return this->swapChainPanel;
}

void RendererD3DServiceData::SetSwapChainPanel(Windows::UI::Xaml::Controls::SwapChainPanel ^v){
	this->swapChainPanel = v;
}

D3D_FEATURE_LEVEL RendererD3DServiceData::GetFeatureLevel() const{
	return this->d3dFeatureLevel;
}

void RendererD3DServiceData::SetFeatureLevel(D3D_FEATURE_LEVEL v){
	this->d3dFeatureLevel = v;
}

Windows::Foundation::Size RendererD3DServiceData::GetRTSize() const{
	return this->d3dRenderTargetSize;
}

void RendererD3DServiceData::SetRTSize(Windows::Foundation::Size v){
	this->d3dRenderTargetSize = v;
}

Windows::Foundation::Size RendererD3DServiceData::GetOutputSize() const{
	return this->outputSize;
}

void RendererD3DServiceData::SetOutputSize(Windows::Foundation::Size v){
	this->outputSize = v;
}

Windows::Foundation::Size RendererD3DServiceData::GetLogicalSize() const{
	return this->logicalSize;
}

void RendererD3DServiceData::SetLogicalSize(Windows::Foundation::Size v){
	this->logicalSize = v;
}

Windows::Graphics::Display::DisplayOrientations RendererD3DServiceData::GetNativeOrientation() const{
	return this->nativeOrientation;
}

void RendererD3DServiceData::SetNativeOrientation(Windows::Graphics::Display::DisplayOrientations v){
	this->nativeOrientation = v;
}

Windows::Graphics::Display::DisplayOrientations RendererD3DServiceData::GetCurrentOrientation() const{
	return this->currentOrientation;
}

void RendererD3DServiceData::SetCurrentOrientation(Windows::Graphics::Display::DisplayOrientations v){
	this->currentOrientation = v;
}

float RendererD3DServiceData::GetDPI() const{
	return this->dpi;
}

void RendererD3DServiceData::SetDPI(float v){
	this->dpi = v;
}

DirectX::XMFLOAT2 RendererD3DServiceData::GetCompositionScale() const{
	return this->compositionScale;
}

void RendererD3DServiceData::SetCompositionScale(DirectX::XMFLOAT2 v){
	this->compositionScale = v;
}

const D2D1::Matrix3x2F &RendererD3DServiceData::GetOrientationTransform2D() const{
	return this->orientationTransform2D;
}

void RendererD3DServiceData::SetOrientationTransform2D(D2D1::Matrix3x2F &v){
	this->orientationTransform2D = v;
}

const DirectX::XMFLOAT4X4 &RendererD3DServiceData::GetOrientationTransform3D() const{
	return this->orientationTransform3D;
}

void RendererD3DServiceData::SetOrientationTransform3D(DirectX::XMFLOAT4X4 &v){
	this->orientationTransform3D = v;
}