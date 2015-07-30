#include "MediaRendererDxResources.h"

MediaRendererDxResources::MediaRendererDxResources(const std::shared_ptr<GuardedDeviceResources> &dx)
	: Geometry(dx), Shader(dx), Sampler(dx){
}