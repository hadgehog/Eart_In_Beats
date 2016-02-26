#pragma once

#include "..\DirectXWinRTHelpers\INativeRenderable.h"
#include "Graphics\Helpers\Timer.h"
#include "VertexTextureNormal.h"
#include "Graphics\Helpers\Thread\PPL\safe_task.h"
#include "..\Shaders\ConstantBufferData.h"
#include "GestureHelper.h"
#include "Graphics\Helpers\Timer.h"

#include <memory>
#include <string>
#include <vector>
#include <cstdint>
#include <condition_variable>
#include <mutex>
#include <stdlib.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>

class EarthRendererNative : public INativeRenderable{
public:
	EarthRendererNative();
	~EarthRendererNative();

	std::function<void(bool)>  showSlidersEvent;
	std::function<void(float)> horizontalManipulationChanged;
	std::function<void(float)> verticalManipulationChanged;

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
	virtual void PointerWheelChanged(Windows::UI::Input::PointerPoint ^ppt) override;

	void LoadModel(const std::string &path);
	void LoadModelTexture(const std::wstring &path);
	void LoadBackgroundTexture(const std::wstring &path);

	bool GetEarthRotationEnabled();
	void SetEarthRotationEnabled(bool v);

	void ResetRotationAngles();

	void SetHorisontalRotationAngle(float angle);	// for model rotating only, no influence on song
	void SetVerticalRotationAngle(float angle);		// for model rotating only, no influence on song

	bool GetManipulationMode();
	void SetPlayingMode(bool v);

private:
	std::shared_ptr<GuardedDeviceResources> dx;

	concurrency::critical_section dataCs;
	concurrency::critical_section externDataCs;

	concurrency::safe_task<void> initializeTask;
	std::mutex initializedMtx;
	std::condition_variable inititalizedCv;
	bool initialized;

	bool modelLoaded;
	bool earthRotationEnabled;
	uint32_t indexCount;
	float scale;
	std::unique_ptr<Timer> tapTimer;
	uint64_t tapTimerPeriod;
	int tapCount;
	bool managingByEarthManipulations;				// enable possibility of manage song by earth manipulations
	float rotationAngleHorizontal;					// for model rotating only, no influence on song
	float rotationAngleVertical;					// for model rotating only, no influence on song
	float angleForRewinding;						// rotating angle for set rewind position
	float angleForVolumeChange;						// rotating angle for change volume
	bool isSongPlayingNow;
	static const float HORIZONTAL_ROTATION_FACTOR;	// rotating factor till song play (may be bigger or less, according to song speed)
	float HORIZONTAL_ANGLE_LIMITER_TOP;
	float HORIZONTAL_ANGLE_LIMITER_BOTTOM;

	GestureHelper ^gestureHelper;

	bool tapOnSphere;
	DirectX::XMVECTOR prevPoint;
	DirectX::XMMATRIX matrixRotation;
	bool showSliders;

	DirectX::XMFLOAT4X4 projection;

	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> normalBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> modelTextureBuffer;

	Microsoft::WRL::ComPtr<ID3D11Buffer> squareVertBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> squareIndexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> squareTextureBuffer;

	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> bgConstantBuffer;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> squareVertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> squarePixelShader;

	Microsoft::WRL::ComPtr<ID3D11PixelShader> bgPixelShader;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayoutBackground;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rsState;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> modelTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> modelTextureView;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> backgroundTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> backgroundTextureView;

	VertexTextureNormal modelPoints;

	ConstantBufferData constantBufferData;
	ConstantBufferData bgConstantBufferData;

	void WaitForInitialization();

	void ProcessMove(const DirectX::XMFLOAT2 &moveVec, const DirectX::XMFLOAT2 &newPos);
	void ProcessZoom(float scale, float x, float y);
	void ProcessRotating(float angle, float x, float y);
	void ManipulationStarted(float x, float y);
	void ManipulationCompleted(const DirectX::XMFLOAT2 &pos);
	void ProcessTap(int tapCount, float x, float y);

	inline bool Intersect(const DirectX::XMVECTOR &rayDir, const DirectX::XMVECTOR &rayOrig,
		const DirectX::XMVECTOR &earthPos, float earthRad,
		DirectX::XMVECTOR &hitPoint, float &distance, DirectX::XMVECTOR &normal);
};

