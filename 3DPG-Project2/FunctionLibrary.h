#pragma once
#include "GameObject.h"

_3DGP_BEGIN_
DX_USE

inline bool SubjectBehindObject(const GameObject* Subject, const GameObject* Object)
{
	//Get the Delta Distance between Subject -> Object
	XMVECTOR Offset = XMVectorSubtract(Object->GetPositionVector(), Subject->GetPositionVector());

	float Dot = XMVectorGetX(XMVector3Dot(Subject->GetLookVector(), Offset));

	/* Angle of Subject Look and Delta distance is less than or
	equal to 90 deg so Object is considered to be infront */
	return Dot >= 0.f;
}

_3DGP_END_
