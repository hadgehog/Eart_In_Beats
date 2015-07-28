#pragma once
#include "..\DirectXCommon\DeviceResources.h"
#include "..\Helpers\H.h"

#include <array>

template<class T>
class CBuffer {
public:
	typedef T Type;

	CBuffer(DX::DeviceResources *dxDev) {
		HRESULT hr = S_OK;
		auto d3dDev = dxDev->GetD3DDevice();
		const int CBufferSizeMultiple = 16;
		D3D11_BUFFER_DESC bufDesc;

		auto roundedBufferSize = H::Math::RoundUp(sizeof(T), CBufferSizeMultiple);

		bufDesc.ByteWidth = static_cast<uint32_t>(roundedBufferSize);
		bufDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
		bufDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
		bufDesc.CPUAccessFlags = 0;
		bufDesc.MiscFlags = 0;
		bufDesc.StructureByteStride = 0;

		hr = d3dDev->CreateBuffer(&bufDesc, nullptr, this->buffer.GetAddressOf());
		H::System::ThrowIfFailed(hr);
	}

	ID3D11Buffer *const *GetBuffers() const {
		return this->buffer.GetAddressOf();
	}

	uint32_t GetBufferCount() const {
		return 1;
	}

	void Update(DX::DeviceResources *dxDev, const T &data) {
		auto d3dCtx = dxDev->GetD3DDeviceContext();

		d3dCtx->UpdateSubresource(this->buffer.Get(), 0, nullptr, &data, sizeof(T), 0);
	}
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
};