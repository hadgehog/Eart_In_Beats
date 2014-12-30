#pragma once

#include "Renderer.h"

#include <map>
#include <memory>
#include <string>

class RendererProxy
{
public:

	RendererProxy();

	~RendererProxy();

	void Initialize(std::map<std::string, std::shared_ptr<void>> params);

private:

	std::unique_ptr<Renderer> rendererImpl;

	void InitializeInternal(std::map<std::string, std::shared_ptr<void>> params);
};