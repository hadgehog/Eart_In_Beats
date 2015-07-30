#pragma once
#include "..\DirectXCommon\DeviceResources.h"
#include "VertexBuffer.h"

class Geometry{
public:
	Geometry(
		D3D11_PRIMITIVE_TOPOLOGY primitiveTopology, 
		VertexBuffer &&vertexBuffer);
	~Geometry();

	uint32_t GetVertexCount() const;

	void SetToContext(DX::DeviceResources *dxDev, uint32_t vertexStartSlot = 0);
private:
	D3D11_PRIMITIVE_TOPOLOGY primitiveTopology;
	VertexBuffer vertexBuffer;
};