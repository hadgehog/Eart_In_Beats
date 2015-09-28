#pragma once

#include <string>
#include <DirectXMath.h>

struct SurfaceMaterial{
	std::wstring matName;
	DirectX::XMFLOAT4 difColor;
	int texArrayIndex;
	bool hasTexture;
	bool transparent;
};