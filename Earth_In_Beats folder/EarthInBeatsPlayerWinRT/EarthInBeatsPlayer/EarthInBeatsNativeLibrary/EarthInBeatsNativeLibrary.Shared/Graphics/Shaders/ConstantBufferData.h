#pragma once

#include <DirectXMath.h>

// Constant buffer used to send MVP matrices to the vertex shader
struct ConstantBufferData {
	DirectX::XMFLOAT4X4 model;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;
};