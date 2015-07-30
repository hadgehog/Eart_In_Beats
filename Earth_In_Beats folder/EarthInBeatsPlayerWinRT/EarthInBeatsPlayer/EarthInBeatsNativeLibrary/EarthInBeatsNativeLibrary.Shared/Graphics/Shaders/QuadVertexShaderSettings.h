#pragma once

#include <DirectXMath.h>

struct QuadVertexShaderSettings {
	DirectX::XMMATRIX mvp;
	DirectX::XMFLOAT2 flip;
	DirectX::XMFLOAT2 leftTopCrop;
	DirectX::XMFLOAT2 sizeCrop;
	DirectX::XMFLOAT2 maskSizeRatio;

	QuadVertexShaderSettings()
		: flip(1.0f, 1.0f), leftTopCrop(0.0f, 0.0f), sizeCrop(1.0f, 1.0f),
		maskSizeRatio(1.0f, 1.0f)
	{
	}
};