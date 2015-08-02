#pragma once

#include "..\DirectXWinRTHelpers\INativeRenderable.h"

struct VertexTextureNormal{
	DirectX::XMFLOAT3 Vertex;
	DirectX::XMFLOAT2 TextureCoord;
	DirectX::XMFLOAT3 Normal;
};