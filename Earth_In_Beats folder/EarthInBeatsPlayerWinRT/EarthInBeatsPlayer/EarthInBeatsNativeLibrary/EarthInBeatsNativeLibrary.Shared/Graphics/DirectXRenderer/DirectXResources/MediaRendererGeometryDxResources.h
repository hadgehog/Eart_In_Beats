#pragma once
#include "AsyncResource.h"
#include "..\Geometry\Geometry.h"
#include "..\DirectXCommon\GuardedDeviceResources.h"

class MediaRendererGeometryDxResources {
public:
	MediaRendererGeometryDxResources(const std::shared_ptr<GuardedDeviceResources> &dx);

	std::shared_ptr<Geometry> GetQuad();
private:
	AsyncResource<std::shared_ptr<Geometry>> quad;

	static std::shared_ptr<Geometry> LoadQuad(const std::shared_ptr<GuardedDeviceResources> &dx);
};