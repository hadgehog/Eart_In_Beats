#include "Geometry.h"

Geometry::Geometry(
	D3D11_PRIMITIVE_TOPOLOGY primitiveTopology,
	VertexBuffer &&vertexBuffer)
	: primitiveTopology(primitiveTopology), vertexBuffer(std::move(vertexBuffer))
{
}

Geometry::~Geometry(){
}

uint32_t Geometry::GetVertexCount() const{
	return this->vertexBuffer.GetVertexCount();
}

void Geometry::SetToContext(DX::DeviceResources *dxDev, uint32_t vertexStartSlot){
	auto d3dCtx = dxDev->GetD3DDeviceContext();

	d3dCtx->IASetPrimitiveTopology(this->primitiveTopology);
	
	this->vertexBuffer.SetToContext(dxDev, vertexStartSlot);
}