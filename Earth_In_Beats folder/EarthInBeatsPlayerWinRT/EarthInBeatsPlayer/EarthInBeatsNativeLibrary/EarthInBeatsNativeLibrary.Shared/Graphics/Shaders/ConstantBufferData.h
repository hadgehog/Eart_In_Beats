#pragma once

#include <DirectXMath.h>

struct ConstantBufferData{
	DirectX::XMFLOAT4X4 World;
	DirectX::XMFLOAT4X4 View;
	DirectX::XMFLOAT4X4 Projection;
};