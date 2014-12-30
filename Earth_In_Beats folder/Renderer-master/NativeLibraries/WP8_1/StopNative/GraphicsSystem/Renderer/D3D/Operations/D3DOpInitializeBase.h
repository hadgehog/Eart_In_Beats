#pragma once
#include "..\RendererD3DServiceDef.h"

class D3DOpInitializeBase : public D3DOperation{
public:
	D3DOpInitializeBase();
	virtual ~D3DOpInitializeBase();

	virtual void Run(D3DServiceData &serviceData) override;
};