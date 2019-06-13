#pragma once
#include "Camera.h"

_3DGP_BEGIN_

class OrbitalCamera : public Camera
{
public:
	OrbitalCamera(Camera* pCamera);
	virtual ~OrbitalCamera();

	virtual void Rotate(float Pitch, float Yaw, float Roll);
	virtual void Update(const DX XMFLOAT3& LookAt, float DeltaTime);
	virtual void SetTarget(const DX XMFLOAT3& LookAt);
};

_3DGP_END_