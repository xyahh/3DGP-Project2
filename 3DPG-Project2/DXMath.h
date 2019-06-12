#pragma once
#include <float.h>

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

DX_END


_3DGP_BEGIN_

inline bool Zero(float fValue, float fEpsilon = FLT_EPSILON)
{
	return  fabsf(fValue) < fEpsilon;
}

inline bool Equal(float a, float b, float Epsilon = FLT_EPSILON)
{
	return(Zero(a - b, Epsilon));
}

inline void Clamp(float lowest, float * n, float highest)
{
	*n = max(lowest, min(*n, highest));
}

inline void Swap(float* a, float* b)
{
	float temp = *a;
	*a = *b;
	*b = temp;
}

inline float InvSqrt(float n)
{
	return 1.f / sqrtf(n);
}

inline bool InRange(float min, float n, float max, bool include_min = true, bool include_max = false)
{
	return !((include_min && n < min)
		|| (!include_min && n <= min)
		|| (include_max && n > max)
		|| (!include_max && n >= max)
		);
}

inline STD string str(DX XMVECTOR_P0 v)
{
	DX XMFLOAT4 f;
	XMStoreFloat4(&f, v);
	return STD string("(" + STD to_string(f.x) + ", " + STD to_string(f.y) + ", " + STD to_string(f.z) + ", " + STD to_string(f.w) + ")");
}

_3DGP_END_