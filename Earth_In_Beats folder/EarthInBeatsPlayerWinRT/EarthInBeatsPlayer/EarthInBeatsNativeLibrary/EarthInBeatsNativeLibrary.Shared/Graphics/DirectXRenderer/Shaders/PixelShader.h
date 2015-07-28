#pragma once
#include "CBuffer.h"
#include "..\DirectXCommon\GuardedDeviceResources.h"
#include "..\Helpers\H.h"

template<class Cb0Data = void>
class PixelShader {
public:
	typedef Cb0Data Cb0Data;
	typedef CBuffer<Cb0Data> Cbuffer;

	template<class T>
	PixelShader(
		ID3D11Device *d3dDev,
		const std::vector<T> &shaderData)
	{
		HRESULT hr = S_OK;

		hr = d3dDev->CreatePixelShader(
			shaderData.data(), shaderData.size(),
			nullptr, this->shader.GetAddressOf());
		H::System::ThrowIfFailed(hr);
	}

	PixelShader(
		ID3D11Device *d3dDev,
		const void *shaderData, size_t shaderDataSize)
	{
		HRESULT hr = S_OK;

		hr = d3dDev->CreatePixelShader(
			shaderData, shaderDataSize,
			nullptr, this->shader.GetAddressOf());
		H::System::ThrowIfFailed(hr);
	}

	void SetToContext(DX::DeviceResources *dxDev, const Cbuffer &cb) {
		auto buffers = cb.GetBuffers();
		auto bufferCount = cb.GetBufferCount();
		auto d3dCtx = dxDev->GetD3DDeviceContext();

		d3dCtx->PSSetShader(this->shader.Get(), nullptr, 0);
		d3dCtx->PSSetConstantBuffers(0, bufferCount, buffers);
	}

private:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> shader;
};

template<>
class PixelShader<void> {
public:
	typedef void Cb0Data;
	typedef void Cbuffer;

	template<class T>
	PixelShader(
		ID3D11Device *d3dDev,
		const std::vector<T> &shaderData)
	{
		HRESULT hr = S_OK;

		hr = d3dDev->CreatePixelShader(
			shaderData.data(), shaderData.size(),
			nullptr, this->shader.GetAddressOf());
		H::System::ThrowIfFailed(hr);
	}

	PixelShader(
		ID3D11Device *d3dDev,
		const void *shaderData, size_t shaderDataSize)
	{
		HRESULT hr = S_OK;

		hr = d3dDev->CreatePixelShader(
			shaderData, shaderDataSize,
			nullptr, this->shader.GetAddressOf());
		H::System::ThrowIfFailed(hr);
	}

	void SetToContext(DX::DeviceResources *dxDev) {
		auto d3dCtx = dxDev->GetD3DDeviceContext();

		d3dCtx->PSSetShader(this->shader.Get(), nullptr, 0);
	}

private:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> shader;
};