#include "pch.h"
#include "RendererD3D.h"
#include "Operations\D3DOperations.h"

RendererD3D::RendererD3D(){
}

RendererD3D::~RendererD3D(){
}

void RendererD3D::InitializeBase(){
	std::unique_ptr<D3DOperation> op = std::unique_ptr<D3DOperation>(new D3DOpInitializeBase);
	this->service.AddOperation(op);
}