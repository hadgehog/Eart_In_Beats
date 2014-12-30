#pragma once
#include "..\Renderer.h"
#include "RendererD3DServiceDef.h"

class RendererD3D : public Renderer{
public:
	RendererD3D();
	virtual ~RendererD3D();

	void InitializeBase();
private:
	D3DService service;
};