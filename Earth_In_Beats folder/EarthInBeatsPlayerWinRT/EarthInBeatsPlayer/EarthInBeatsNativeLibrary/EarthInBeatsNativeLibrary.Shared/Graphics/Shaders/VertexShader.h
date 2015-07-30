#pragma once
#include "CBuffer.h"
#include "..\DirectXCommon\GuardedDeviceResources.h"
#include "..\Helpers\H.h"

template<class Cb0Data = void>
class VertexShader {
public:
	typedef Cb0Data Cb0Data;
	typedef CBuffer<Cb0Data> Cbuffer;

	template<class T>
	VertexShader(
		ID3D11Device *d3dDev,
		const std::vector<T> &shaderData,
		D3D11_INPUT_ELEMENT_DESC *inputDesc, size_t inputDescCount)
	{
		HRESULT hr = S_OK;

		hr = d3dDev->CreateVertexShader(
			shaderData.data(), shaderData.size(), 
			nullptr, this->shader.GetAddressOf());
		H::System::ThrowIfFailed(hr);

		hr = d3dDev->CreateInputLayout(
			inputDesc, static_cast<uint32_t>(inputDescCount), 
			shaderData.data(), shaderData.size(), 
			this->inputLayout.GetAddressOf());
		H::System::ThrowIfFailed(hr);
	}

	VertexShader(
		ID3D11Device *d3dDev,
		const void *shaderData, size_t shaderDataSize, 
		D3D11_INPUT_ELEMENT_DESC *inputDesc, size_t inputDescCount) 
	{
		HRESULT hr = S_OK;

		hr = d3dDev->CreateVertexShader(
			shaderData, shaderDataSize, 
			nullptr, this->shader.GetAddressOf());
		H::System::ThrowIfFailed(hr);

		hr = d3dDev->CreateInputLayout(
			inputDesc, static_cast<uint32_t>(inputDescCount), 
			shaderData, shaderDataSize, 
			this->inputLayout.GetAddressOf());
		H::System::ThrowIfFailed(hr);
	}

	void SetToContext(DX::DeviceResources *dxDev, const Cbuffer &cb) {
		auto buffers = cb.GetBuffers();
		auto bufferCount = cb.GetBufferCount();
		auto d3dCtx = dxDev->GetD3DDeviceContext();

		d3dCtx->IASetInputLayout(this->inputLayout.Get());
		d3dCtx->VSSetShader(this->shader.Get(), nullptr, 0);
		d3dCtx->VSSetConstantBuffers(0, bufferCount, buffers);
	}

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
};

template<>
class VertexShader<void> {
public:
	typedef void Cb0Data;
	typedef void Cbuffer;

	template<class T>
	VertexShader(
		ID3D11Device *d3dDev,
		const std::vector<T> &shaderData,
		D3D11_INPUT_ELEMENT_DESC *inputDesc, size_t inputDescCount)
	{
		HRESULT hr = S_OK;

		hr = d3dDev->CreateVertexShader(
			shaderData.data(), shaderData.size(),
			nullptr, this->shader.GetAddressOf());
		H::System::ThrowIfFailed(hr);

		hr = d3dDev->CreateInputLayout(
			inputDesc, static_cast<uint32_t>(inputDescCount),
			shaderData.data(), shaderData.size(),
			this->inputLayout.GetAddressOf());
		H::System::ThrowIfFailed(hr);
	}

	VertexShader(
		ID3D11Device *d3dDev,
		const void *shaderData, size_t shaderDataSize,
		D3D11_INPUT_ELEMENT_DESC *inputDesc, size_t inputDescCount)
	{
		HRESULT hr = S_OK;

		hr = d3dDev->CreateVertexShader(
			shaderData, shaderDataSize,
			nullptr, this->shader.GetAddressOf());
		H::System::ThrowIfFailed(hr);

		hr = d3dDev->CreateInputLayout(
			inputDesc, static_cast<uint32_t>(inputDescCount),
			shaderData, shaderDataSize,
			this->inputLayout.GetAddressOf());
		H::System::ThrowIfFailed(hr);
	}

	void SetToContext(DX::DeviceResources *dxDev) {
		auto d3dCtx = dxDev->GetD3DDeviceContext();

		d3dCtx->IASetInputLayout(this->inputLayout.Get());
		d3dCtx->VSSetShader(this->shader.Get(), nullptr, 0);
	}

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
};