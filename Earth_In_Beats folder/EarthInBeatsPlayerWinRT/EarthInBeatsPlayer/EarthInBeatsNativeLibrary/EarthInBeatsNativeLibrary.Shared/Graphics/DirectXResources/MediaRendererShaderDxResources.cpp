#include "MediaRendererShaderDxResources.h"

MediaRendererShaderDxResources::MediaRendererShaderDxResources(const std::shared_ptr<GuardedDeviceResources> &dx)
	: quadVs(MediaRendererShaderDxResources::LoadQuadVertexShader, dx),
	quadPs(MediaRendererShaderDxResources::LoadPixelShader<QuadPs>, dx, L"EarthInBeatsNativeLibrary\\QuadPixelShader.cso")
{
}

MediaRendererShaderDxResources::~MediaRendererShaderDxResources() {
}

std::shared_ptr<QuadVs> MediaRendererShaderDxResources::GetQuadVs() {
	return this->quadVs.Get();
}

std::shared_ptr<QuadPs> MediaRendererShaderDxResources::GetQuadPs() {
	return this->quadPs.Get();
}

std::shared_ptr<QuadVs> MediaRendererShaderDxResources::LoadQuadVertexShader(const std::shared_ptr<GuardedDeviceResources> &dx) {
	D3D11_INPUT_ELEMENT_DESC elementDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	auto shaderData = H::System::LoadPackageFile(L"EarthInBeatsNativeLibrary\\QuadVertexShader.cso");

	auto dxDevMt = dx->GetMultithread();
	std::shared_ptr<QuadVs> shader =
		std::make_shared<QuadVs>(dxDevMt.GetD3DDevice(), shaderData, elementDesc, ARRAY_SIZE(elementDesc));

	return shader;
}