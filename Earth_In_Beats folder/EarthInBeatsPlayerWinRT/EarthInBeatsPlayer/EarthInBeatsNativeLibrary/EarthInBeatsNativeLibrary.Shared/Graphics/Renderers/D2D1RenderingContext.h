#pragma once

#include "..\DirectXResources\Resources.h"

#include <d2d1.h>
#include <d3d11_2.h>
#include <d2d1_2.h>
#include <dwrite_2.h>
#include <wincodec.h>
#include <stdint.h>

struct D2D1RenderingContext{
	ID2D1RenderTarget *D2DRenderTarget;
	ID2D1Factory *D2DFactory;
	IDWriteFactory2 *D2DWriteFactory;
	D2D1_MATRIX_3X2_F Projection;
	float DPI;
	DirectX::XMFLOAT2 scale;
	int64_t currentVideoTime;
};