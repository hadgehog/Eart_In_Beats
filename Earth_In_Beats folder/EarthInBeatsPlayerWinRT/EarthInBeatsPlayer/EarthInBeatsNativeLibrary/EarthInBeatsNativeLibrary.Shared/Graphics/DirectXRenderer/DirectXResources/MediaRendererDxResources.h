#pragma once
#include "MediaRendererGeometryDxResources.h"
#include "MediaRendererShaderDxResources.h"
#include "MediaRendererSamplerDxResources.h"
#include "..\DirectXCommon\GuardedDeviceResources.h"

class MediaRendererDxResources {
public:
	MediaRendererGeometryDxResources Geometry;
	MediaRendererShaderDxResources Shader;
	MediaRendererSamplerDxResources Sampler;

	MediaRendererDxResources(const std::shared_ptr<GuardedDeviceResources> &dx);
};