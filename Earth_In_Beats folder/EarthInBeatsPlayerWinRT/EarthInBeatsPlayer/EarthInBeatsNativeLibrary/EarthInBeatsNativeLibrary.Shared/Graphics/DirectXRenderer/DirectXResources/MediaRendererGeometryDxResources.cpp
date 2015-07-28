#include "MediaRendererGeometryDxResources.h"

#include "..\Helpers\H.h"

MediaRendererGeometryDxResources::MediaRendererGeometryDxResources(const std::shared_ptr<GuardedDeviceResources> &dx)
	: quad(MediaRendererGeometryDxResources::LoadQuad, dx) {
}

std::shared_ptr<Geometry> MediaRendererGeometryDxResources::GetQuad() {
	return this->quad.Get();
}

std::shared_ptr<Geometry> MediaRendererGeometryDxResources::LoadQuad(const std::shared_ptr<GuardedDeviceResources> &dx) {
	HRESULT hr = S_OK;
	D3D11_BUFFER_DESC bufDesc;
	D3D11_SUBRESOURCE_DATA bufData;
	Microsoft::WRL::ComPtr<ID3D11Buffer> posBuffer;

	DirectX::XMFLOAT2 posData[] = {
		DirectX::XMFLOAT2(-0.5f, 0.5f),
		DirectX::XMFLOAT2(0.5f, 0.5f),
		DirectX::XMFLOAT2(-0.5f, -0.5f),
		DirectX::XMFLOAT2(0.5f, -0.5f)
	};

	bufDesc.ByteWidth = sizeof posData;
	bufDesc.Usage = D3D11_USAGE::D3D11_USAGE_IMMUTABLE;
	bufDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
	bufDesc.CPUAccessFlags = 0;
	bufDesc.MiscFlags = 0;
	bufDesc.StructureByteStride = 0;

	bufData.pSysMem = posData;
	bufData.SysMemPitch = 0;
	bufData.SysMemSlicePitch = 0;

	auto dxDevMt = dx->GetMultithread();
	auto d3dDev = dxDevMt.GetD3DDevice();
	hr = d3dDev->CreateBuffer(&bufDesc, &bufData, posBuffer.GetAddressOf());
	H::System::ThrowIfFailed(hr);

	VertexBuffer vBuf(ARRAY_SIZE(posData));

	vBuf.Add(posBuffer, sizeof(DirectX::XMFLOAT2));

	std::shared_ptr<Geometry> geom =
		std::make_shared<Geometry>(
			D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
			std::move(vBuf));

	return geom;
}