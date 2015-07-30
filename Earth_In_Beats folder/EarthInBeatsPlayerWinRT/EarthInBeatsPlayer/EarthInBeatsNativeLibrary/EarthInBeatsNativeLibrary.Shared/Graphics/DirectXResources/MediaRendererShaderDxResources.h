#pragma once
#include "AsyncResource.h"
#include "..\Shaders\VertexShader.h"
#include "..\Shaders\PixelShader.h"
#include "..\Shaders\QuadVertexShaderSettings.h"
#include "..\DirectXCommon\GuardedDeviceResources.h"
#include "..\Helpers\H.h"

#include <memory>
#include <DirectXMath.h>

#pragma region SHADERS_DEFINES
typedef VertexShader<QuadVertexShaderSettings> QuadVs;
typedef PixelShader<> QuadPs;
#pragma endregion

class MediaRendererShaderDxResources {
public:
	MediaRendererShaderDxResources(const std::shared_ptr<GuardedDeviceResources> &dx);
	~MediaRendererShaderDxResources();

#pragma region SHADERS_GETTERS
	std::shared_ptr<QuadVs> GetQuadVs();
	std::shared_ptr<QuadPs> GetQuadPs();
#pragma endregion

private:
#pragma region SHADERS
	AsyncResource<std::shared_ptr<QuadVs>>	quadVs;
	AsyncResource<std::shared_ptr<QuadPs>>	quadPs;
#pragma endregion

	static std::shared_ptr<QuadVs> LoadQuadVertexShader(const std::shared_ptr<GuardedDeviceResources> &dx);

	template<class Ps>
	static std::shared_ptr<Ps> LoadPixelShader(const std::shared_ptr<GuardedDeviceResources> &dx, const wchar_t *path) {
		auto shaderData = H::System::LoadPackageFile(path);

		auto dxDevMt = dx->GetMultithread();
		std::shared_ptr<Ps> shader =
			std::make_shared<Ps>(dxDevMt.GetD3DDevice(), shaderData);

		return shader;
	}
};

