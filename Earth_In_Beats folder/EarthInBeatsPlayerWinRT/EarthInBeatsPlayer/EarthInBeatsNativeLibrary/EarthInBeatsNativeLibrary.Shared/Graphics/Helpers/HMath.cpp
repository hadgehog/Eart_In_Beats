#include "HMath.h"

using namespace DirectX;

DirectX::XMFLOAT2 HMath::InscribeRect(const DirectX::XMFLOAT2 &sourRect, const DirectX::XMFLOAT2 &destRect){
	DirectX::XMFLOAT2 inscribed = sourRect;
	bool widthLess = sourRect.x < destRect.x;
	bool heightLess = sourRect.y < destRect.y;
	bool hardCase = !widthLess && !heightLess;

	if (hardCase){
		if (sourRect.x > sourRect.y){
			float invAr = sourRect.y / sourRect.x;

			inscribed.x = destRect.x;
			inscribed.y = destRect.x * invAr;

			if (inscribed.y > destRect.y){
				float hscale = destRect.y / inscribed.y;

				inscribed.x *= hscale;
				inscribed.y *= hscale;
			}
		}
		else{
			float ar = sourRect.x / sourRect.y;

			inscribed.x = destRect.y * ar;
			inscribed.y = destRect.y;

			if (inscribed.x > destRect.x){
				float wscale = destRect.x / inscribed.x;

				inscribed.x *= wscale;
				inscribed.y *= wscale;
			}
		}
	}
	else{
		if (widthLess && !heightLess){
			float hscale = destRect.y / sourRect.y;

			inscribed.x *= hscale;
			inscribed.y *= hscale;
		}

		if (!widthLess && heightLess){
			float wscale = destRect.x / sourRect.x;

			inscribed.x *= wscale;
			inscribed.y *= wscale;
		}
	}

	return inscribed;
}

void HMath::BoundingOrientedBoxTransform(const DirectX::BoundingOrientedBox &_this, DirectX::BoundingOrientedBox &Out, DirectX::CXMMATRIX M){
	// Load the box.
	XMVECTOR vCenter = XMLoadFloat3(&_this.Center);
	XMVECTOR vExtents = XMLoadFloat3(&_this.Extents);
	XMVECTOR vOrientation = XMLoadFloat4(&_this.Orientation);

	assert(DirectX::Internal::XMQuaternionIsUnit(vOrientation));

	// Composite the box rotation and the transform rotation.
	XMMATRIX nM;
	nM.r[0] = XMVector3Normalize(M.r[0]);
	nM.r[1] = XMVector3Normalize(M.r[1]);
	nM.r[2] = XMVector3Normalize(M.r[2]);
	nM.r[3] = g_XMIdentityR3;
	XMVECTOR Rotation = XMQuaternionRotationMatrix(nM);
	vOrientation = XMQuaternionMultiply(vOrientation, Rotation);

	// Transform the center.
	vCenter = XMVector3Transform(vCenter, M);

	// Scale the box extents.
	XMVECTOR dX = XMVector3Length(M.r[0]);
	XMVECTOR dY = XMVector3Length(M.r[1]);
	XMVECTOR dZ = XMVector3Length(M.r[2]);

	// bug:
	/*XMVECTOR VectorScale = XMVectorSelect(dX, dY, g_XMSelect1000);
	VectorScale = XMVectorSelect(VectorScale, dZ, g_XMSelect1100);*/
	// fix:
	XMVECTOR VectorScale = XMVectorSelect(dY, dX, g_XMSelect1000);
	VectorScale = XMVectorSelect(dZ, VectorScale, g_XMSelect1100);
	vExtents = vExtents * VectorScale;

	// Store the box.
	XMStoreFloat3(&Out.Center, vCenter);
	XMStoreFloat3(&Out.Extents, vExtents);
	XMStoreFloat4(&Out.Orientation, vOrientation);
}

DirectX::ContainmentType HMath::BoundingBoxContains(const DirectX::BoundingBox &_this, const DirectX::BoundingOrientedBox& box){
	if (!box.Intersects(_this))
		return DISJOINT;

	XMVECTOR vCenter = XMLoadFloat3(&_this.Center);
	XMVECTOR vExtents = XMLoadFloat3(&_this.Extents);

	// Subtract off the AABB center to remove a subtract below
	XMVECTOR oCenter = XMLoadFloat3(&box.Center) - vCenter;

	XMVECTOR oExtents = XMLoadFloat3(&box.Extents);
	XMVECTOR oOrientation = XMLoadFloat4(&box.Orientation);

	assert(DirectX::Internal::XMQuaternionIsUnit(oOrientation));

	XMVECTOR Inside = XMVectorTrueInt();

	for (size_t i = 0; i < BoundingOrientedBox::CORNER_COUNT; ++i)
	{
		XMVECTOR C = XMVector3Rotate(oExtents * g_BoxOffset[i], oOrientation) + oCenter;
		XMVECTOR d = XMVectorAbs(C);
		Inside = XMVectorAndInt(Inside, XMVectorLessOrEqual(d, vExtents));
	}

	return (XMVector3EqualInt(Inside, XMVectorTrueInt())) ? CONTAINS : INTERSECTS;
}

DirectX::XMMATRIX HMath::XMMatrixFrom(const D2D1_MATRIX_3X2_F &v){
	DirectX::XMMATRIX m = DirectX::XMMatrixIdentity();

	m.r[0].XF = v._11;
	m.r[0].YF = v._12;

	m.r[1].XF = v._21;
	m.r[1].YF = v._22;

	m.r[3].XF = v._31;
	m.r[3].YF = v._32;

	return m;
}

DirectX::XMVECTOR HMath::GetAABBSizeFrom(DirectX::CXMMATRIX matrix){
	DirectX::XMVECTOR axisX = DirectX::XMVectorAbs(matrix.r[0]);
	DirectX::XMVECTOR axisY = DirectX::XMVectorAbs(matrix.r[1]);
	DirectX::XMVECTOR axisZ = DirectX::XMVectorAbs(matrix.r[2]);

	DirectX::XMVECTOR addXY = DirectX::XMVectorAdd(axisX, axisY);
	DirectX::XMVECTOR addXYZ = DirectX::XMVectorAdd(addXY, axisZ);

	return addXYZ;
}