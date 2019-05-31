#pragma once
#include "GameObject.h"

_3DGP_BEGIN_

class RotatingObject : public GameObject
{
public:
	RotatingObject();
	~RotatingObject();

	virtual void Update(float DeltaTime);

protected:
	DX XMFLOAT3 m_RotationAxis;
	float		m_AngularSpeed;
};

_3DGP_END_

