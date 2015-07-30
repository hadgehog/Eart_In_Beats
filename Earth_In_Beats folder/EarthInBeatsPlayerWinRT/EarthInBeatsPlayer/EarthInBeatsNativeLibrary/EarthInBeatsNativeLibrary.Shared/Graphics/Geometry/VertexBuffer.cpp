#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(uint32_t vertexCount)
	: vertexCount(vertexCount){
}

uint32_t VertexBuffer::GetVertexCount() const{
	return this->vertexCount;
}

void VertexBuffer::Add(const Microsoft::WRL::ComPtr<ID3D11Buffer> &buffer, uint32_t stride){
	this->buffer.push_back(buffer);
	this->stride.push_back(stride);
	this->offset.push_back(0);
}

void VertexBuffer::SetToContext(DX::DeviceResources *dxDev, uint32_t vertexStartSlot){
	auto d3dCtx = dxDev->GetD3DDeviceContext();

	d3dCtx->IASetVertexBuffers(
		vertexStartSlot, 
		(uint32_t)this->buffer.size(), 
		this->buffer.data(), 
		this->stride.data(), 
		this->offset.data());
}