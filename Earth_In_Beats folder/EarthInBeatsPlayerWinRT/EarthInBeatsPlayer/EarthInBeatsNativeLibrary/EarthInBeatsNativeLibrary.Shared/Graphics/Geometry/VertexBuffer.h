#pragma once
#include "..\DirectXCommon\DeviceResources.h"
#include "..\Helpers\comptr_vector.h"

#include <vector>

class VertexBuffer{
public:
	VertexBuffer(uint32_t vertexCount);

	uint32_t GetVertexCount() const;
	//void SetVertexCount(uint32_t v);

	void Add(const Microsoft::WRL::ComPtr<ID3D11Buffer> &buffer, uint32_t stride);

	void SetToContext(DX::DeviceResources *dxDev, uint32_t vertexStartSlot);
private:
	comptr_vector<ID3D11Buffer> buffer;
	std::vector<uint32_t> stride;
	std::vector<uint32_t> offset;
	uint32_t vertexCount;
};