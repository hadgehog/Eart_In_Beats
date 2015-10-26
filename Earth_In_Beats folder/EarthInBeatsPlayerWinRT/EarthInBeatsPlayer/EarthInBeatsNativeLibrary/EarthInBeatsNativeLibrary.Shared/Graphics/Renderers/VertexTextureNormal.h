#pragma once

#include <DirectXMath.h>

struct VertexTextureNormal{
	std::vector<DirectX::XMFLOAT3> Vertex;
	std::vector<DirectX::XMFLOAT2> TextureCoord;
	std::vector<DirectX::XMFLOAT3> Normal;
};