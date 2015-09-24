#include "DxRenderingContext.h"

DxRenderingContext::DxRenderingContext(DX::DeviceResources *dxDev, int64_t ptsHns, DirectX::CXMMATRIX projection, float timeDelta, float timeFraction)
	: dxDev(dxDev), ptsHns(ptsHns), projection(projection), timeDelta(timeDelta), timeFraction(timeFraction){
}

DxRenderingContext::DxRenderingContext(const DxRenderingContext &other, DirectX::CXMMATRIX projection)
	: dxDev(other.dxDev), ptsHns(other.ptsHns), projection(projection), timeDelta(other.timeDelta), timeFraction(other.timeFraction){
}

DxRenderingContext::~DxRenderingContext() {
}

DX::DeviceResources *DxRenderingContext::GetDxDev() const {
	return this->dxDev;
}

int64_t DxRenderingContext::GetPtsHns() const {
	return this->ptsHns;
}

DirectX::CXMMATRIX DxRenderingContext::GetProjection() const {
	return this->projection;
}

float DxRenderingContext::GetTimeDelta() const {
	return this->timeDelta;
}

float DxRenderingContext::GetTimeFraction() const {
	return this->timeFraction;
}

DirectX::XMFLOAT2 DxRenderingContext::GetCurrentViewportSize() const {
	DirectX::XMFLOAT2 size;
	D3D11_VIEWPORT viewport;
	uint32_t viewportCount = 1;
	auto d3dCtx = this->dxDev->GetD3DDeviceContext();

	d3dCtx->RSGetViewports(&viewportCount, &viewport);

	size.x = viewport.Width;
	size.y = viewport.Height;

	return size;
}