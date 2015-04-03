#pragma once

#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <d2d1.h>

// Defines for accessing XMVECTOR components without XMVectorGet.. , XMVectorSet.. methods
#ifdef _M_ARM

#define XF n128_f32[0]
#define YF n128_f32[1]
#define ZF n128_f32[2]
#define WF n128_f32[3]

#define XU32 n128_u32[0]
#define YU32 n128_u32[1]
#define ZU32 n128_u32[2]
#define WU32 n128_u32[3]

#define XI32 n128_i32[0]
#define YI32 n128_i32[1]
#define ZI32 n128_i32[2]
#define WI32 n128_i32[3]

#define F32 n128_f32

#define I8 n128_i8
#define I16 n128_i16
#define I32 n128_i32
#define I64 n128_i64

#define U8 n128_u8
#define U16 n128_u16
#define U32 n128_u32
#define U64 n128_u64

#else

#define XF m128_f32[0]
#define YF m128_f32[1]
#define ZF m128_f32[2]
#define WF m128_f32[3]

#define XU32 m128_u32[0]
#define YU32 m128_u32[1]
#define ZU32 m128_u32[2]
#define WU32 m128_u32[3]

#define XI32 m128_i32[0]
#define YI32 m128_i32[1]
#define ZI32 m128_i32[2]
#define WI32 m128_i32[3]

#define F32 m128_f32

#define I8 m128_i8
#define I16 m128_i16
#define I32 m128_i32
#define I64 m128_i64

#define U8 m128_u8
#define U16 m128_u16
#define U32 m128_u32
#define U64 m128_u64

#endif

class HMath{
public:

	/// <summary>
	/// Inscribes source rectangle into destination rectangle.
	/// If source rectangle is bigger than destination one then
	/// inscribed will be rectangle with maximum size that can
	/// be in destination rectangle and same aspect ratio as source rectangle.
	/// </summary>
	/// <param name="sourRect">Rectangle that need to inscribed.</param>
	/// <param name="destRect">Rectangle in which sourRect will bee inscribed.</param>
	/// <returns>Returns inscribed rectangle.</returns>
	static DirectX::XMFLOAT2 InscribeRect(const DirectX::XMFLOAT2 &sourRect, const DirectX::XMFLOAT2 &destRect);

	/*
	DirectXMath have bug in BoundingOrientedBox::Transform method:
	http://xboxforums.create.msdn.com/forums/p/113061/680807.aspx
	This implementation have fix for bug.
	*/
	static void BoundingOrientedBoxTransform(const DirectX::BoundingOrientedBox &_this, DirectX::BoundingOrientedBox &Out, DirectX::CXMMATRIX M);
	static DirectX::ContainmentType BoundingBoxContains(const DirectX::BoundingBox &_this, const DirectX::BoundingOrientedBox& box);

	static DirectX::XMMATRIX XMMatrixFrom(const D2D1_MATRIX_3X2_F &v);

	static DirectX::XMVECTOR GetAABBSizeFrom(DirectX::CXMMATRIX matrix);
};