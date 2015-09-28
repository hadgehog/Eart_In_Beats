#pragma once

#include "..\DirectXCommon\DeviceResources.h"
#include <DirectXMath.h>

class DxRenderingContext {
public:
	DxRenderingContext(DX::DeviceResources *dxDev, int64_t ptsHns, DirectX::CXMMATRIX projection, float timeDelta, float timeFraction);
	DxRenderingContext(const DxRenderingContext &other, DirectX::CXMMATRIX projection);
	~DxRenderingContext();

	DX::DeviceResources *GetDxDev() const;
	int64_t GetPtsHns() const;
	DirectX::CXMMATRIX GetProjection() const;
	float GetTimeDelta() const;
	float GetTimeFraction() const;

	DirectX::XMFLOAT2 GetCurrentViewportSize() const;

private:
	DX::DeviceResources *dxDev;
	int64_t ptsHns;
	DirectX::XMMATRIX projection;
	float timeDelta;
	float timeFraction;
};