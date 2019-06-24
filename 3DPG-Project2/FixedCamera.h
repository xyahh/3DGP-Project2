#pragma once
#include "Camera.h"

_3DGP_BEGIN_

class FixedCamera : public Camera
{
public:
	FixedCamera(Camera* pCamera);
	virtual ~FixedCamera();

	virtual void SetTarget(const DX XMFLOAT3& LookAt);
};

_3DGP_END_
