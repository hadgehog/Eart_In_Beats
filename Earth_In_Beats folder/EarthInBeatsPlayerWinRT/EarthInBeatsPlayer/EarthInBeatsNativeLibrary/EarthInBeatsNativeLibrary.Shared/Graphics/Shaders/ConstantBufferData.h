#pragma once

#include <DirectXMath.h>

struct ConstantBufferData {
	DirectX::XMMATRIX  MVP;

	//These will be used for the pixel shader
	//DirectX::XMFLOAT4 color;
	//bool hasTexture;
};