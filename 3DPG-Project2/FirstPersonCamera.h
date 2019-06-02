#pragma once
#include "Camera.h"

_3DGP_BEGIN_

class FirstPersonCamera : public Camera
{
public:
	FirstPersonCamera(Camera* pCamera);
	~FirstPersonCamera();

	virtual void Rotate(float Pitch, float Yaw, float Roll);
};

_3DGP_END_

