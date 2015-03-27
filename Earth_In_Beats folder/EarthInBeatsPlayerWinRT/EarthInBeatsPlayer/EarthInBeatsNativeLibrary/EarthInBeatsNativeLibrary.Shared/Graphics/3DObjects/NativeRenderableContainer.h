#pragma once

#include"INativeRenderable.h"

public ref class NativeRenderableContainer sealed
{
public:
	NativeRenderableContainer();
	virtual ~NativeRenderableContainer();

internal:
	void Set(const std::shared_ptr<INativeRenderable> &nativeRender);
	std::shared_ptr<INativeRenderable> Get();

private:
	std::shared_ptr<INativeRenderable> nativeRender;
};

