#pragma once
#include "Camera.h"

_3DGP_BEGIN_

class ThirdPersonCamera : public Camera
{
public:
	ThirdPersonCamera(Camera* pCamera);
	virtual ~ThirdPersonCamera();

	virtual void Update(const DX XMFLOAT3& LookAt, float DeltaTime);

	virtual void SetTarget(const DX XMFLOAT3& LookAt);
};

_3DGP_END_
