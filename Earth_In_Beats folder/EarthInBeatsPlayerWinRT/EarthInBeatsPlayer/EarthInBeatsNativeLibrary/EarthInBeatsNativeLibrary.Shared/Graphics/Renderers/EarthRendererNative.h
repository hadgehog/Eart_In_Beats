#pragma once

#include "..\DirectXWinRTHelpers\INativeRenderable.h"
#include "Graphics\Helpers\Timer.h"
#include "VertexTextureNormal.h"
#include "Graphics\Helpers\Thread\PPL\safe_task.h"
#include "..\Shaders\ConstantBufferData.h"
#include "..\Shaders\SurfaceMaterial.h"


#include <memory>
#include <string>
#include <vector>
#include <cstdint>
#include <condition_variable>
#include <mutex>
#include <stdlib.h>

class EarthRendererNative : public INativeRenderable{
public:
	EarthRendererNative();
	~EarthRendererNative();

	virtual void Initialize(const std::shared_ptr<GuardedDeviceResources> &dx) override;

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

	void LoadModel(std::string path);

private:
	std::shared_ptr<GuardedDeviceResources> dx;

	concurrency::critical_section dataCs;

	concurrency::safe_task<void> initializeTask;
	std::mutex initializedMtx;
	std::condition_variable inititalizedCv;
	bool initialized;

	bool modelLoaded;

	DirectX::XMFLOAT4X4 projection;
	Microsoft::WRL::ComPtr<ID3D11Buffer> d2dVertBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> d2dIndexBuffer;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> PS;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> VS;
	Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> quadSampler;

	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> meshSRV;
	std::vector<std::wstring> textureNameArray;

	std::vector<VertexTextureNormal> modelPoints;
	std::vector<SurfaceMaterial> materials;

	void WaitForInitialization();
};

