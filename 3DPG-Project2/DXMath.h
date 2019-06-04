#pragma once
#include <float.h>

inline bool Zero(float fValue)
{
	return  fabsf(fValue) < FLT_EPSILON;
}

inline void Clamp(float lowest, float * n, float highest)
{
	*n = max(lowest, min(*n, highest));
}

DX_BEGIN

using XMVECTOR_P0 = DX FXMVECTOR;
using XMVECTOR_P1 = DX FXMVECTOR;
using XMVECTOR_P2 = DX FXMVECTOR;
using XMVECTOR_P3 = DX GXMVECTOR;
using XMVECTOR_P4 = DX HXMVECTOR;
using XMVECTOR_P5 = DX HXMVECTOR;
using XMVECTOR_PX = DX CXMVECTOR;

inline STD ostream& XM_CALLCONV operator<<(STD ostream& out, XMVECTOR_P0 v)
{
	out << "(" << XMVectorGetX(v) << ", " << XMVectorGetY(v) << ", " << XMVectorGetZ(v) << ", " << XMVectorGetW(v) << ")";
	return out;
}

inline bool XM_CALLCONV AnyTrue(XMVECTOR_P0 v)
{
	uint32_t out;
	XMVectorEqualIntR(&out, XMVectorSwizzle(v, 0, 1, 2, 0), XMVectorTrueInt());
	return XMComparisonAnyTrue(out);
}

/*
 *
 *
 */
inline XMVECTOR XM_CALLCONV ToBool(XMVECTOR_P0 v)
{
	return XMVectorClamp
	(
		XMVectorNotEqual(v, XMVectorZero()),
		XMVectorZero(),
		XMVectorSplatOne()
	);
}


DX_END