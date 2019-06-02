#pragma once
#include "Camera.h"

_3DGP_BEGIN_

class SpaceshipCamera : public Camera
{
public:
	SpaceshipCamera(Camera* pCamera);
	virtual ~SpaceshipCamera();

	virtual void Rotate(float Pitch, float Yaw, float Roll);
};

_3DGP_END_
