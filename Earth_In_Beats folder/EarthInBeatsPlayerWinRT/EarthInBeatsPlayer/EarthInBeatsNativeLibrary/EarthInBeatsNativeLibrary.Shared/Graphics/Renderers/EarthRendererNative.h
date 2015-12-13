#pragma once

#include "..\DirectXWinRTHelpers\INativeRenderable.h"
#include "Graphics\Helpers\Timer.h"
#include "VertexTextureNormal.h"
#include "Graphics\Helpers\Thread\PPL\safe_task.h"
#include "..\Shaders\ConstantBufferData.h"
#include "..\Shaders\ShaderStructures.h"
#include "..\Shaders\PsConstBufferData.h"

#include <memory>
#include <string>
#include <vector>
#include <cstdint>
#include <condition_variable>
#include <mutex>
#include <stdlib.h>
#include <d3d11.h>

class EarthRendererNative : public INativeRenderable{
public:
	EarthRendererNative();
	~EarthRendererNative();

	virtual void Initialize(const std::shared_ptr<GuardedDeviceResources> &dx) override;
	void Shutdown() override;

	virtual void CreateDeviceDependentResources() override;
	virtual void ReleaseDeviceDependentResources() override;

	virtual void CreateSizeDependentResources() override;

	virtual void OnRenderThreadStart() override;
	virtual void OnRenderThreadEnd() override;

	virtual void Update(const DX::StepTimer &timer) override;
	virtual void Render() override;

	virtual void PointerPressed(Windows::UI::Input::PointerPoint ^ppt) override;
	virtual void PointerMoved(Windows::UI::Input::PointerPoint ^ppt) override;
	virtual void PointerReleased(Windows::UI::Input::PointerPoint ^ppt) override;

	void LoadModel(const std::string &path);
	void LoadModelTexture(const std::wstring &path);
	void LoadBackgroundTexture(const std::wstring &path);

	bool GetEarthRotationEnabled();
	void SetEarthRotationEnabled(bool v);

private:
	std::shared_ptr<GuardedDeviceResources> dx;

	concurrency::critical_section dataCs;
	concurrency::critical_section externDataCs;

	concurrency::safe_task<void> initializeTask;
	std::mutex initializedMtx;
	std::condition_variable inititalizedCv;
	bool initialized;

	bool modelLoaded;
	float rotationAngle;
	bool earthRotationEnabled;
	uint32_t indexCount;

	DirectX::XMFLOAT4X4 projection;

	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> normalBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> matrixBuffer;

	Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rsState;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView;

	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> meshSRV;
	std::vector<std::wstring> textureNameArray;

	VertexTextureNormal modelPoints;

	EarthInBeatsNativeLibrary::MVPConstantBuffer constantBufferData;

	void WaitForInitialization();
};

