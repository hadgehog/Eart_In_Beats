#pragma once

#include "DirectXWinRTHelpers\INativeRenderable.h"

struct VertexTextureNormal{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT2 TextureCoord;
	DirectX::XMFLOAT3 Normal;
};