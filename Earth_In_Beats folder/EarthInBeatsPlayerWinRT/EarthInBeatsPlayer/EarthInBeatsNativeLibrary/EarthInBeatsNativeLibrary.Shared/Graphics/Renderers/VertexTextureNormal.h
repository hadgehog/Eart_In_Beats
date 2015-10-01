#pragma once

#include <DirectXMath.h>

struct VertexTextureNormal{
	VertexTextureNormal() {}
	VertexTextureNormal(float x, float y, float z, float u, float v, 
		float nx, float ny, float nz) : Vertex(x, y, z), TextureCoord(u, v), Normal(nx, ny, nz) {}

	DirectX::XMFLOAT3 Vertex;
	DirectX::XMFLOAT2 TextureCoord;
	DirectX::XMFLOAT3 Normal;
};