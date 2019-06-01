#pragma once
#include "GameObject.h"

_3DGP_BEGIN_

class RotatingObject : public GameObject
{
public:
	RotatingObject();
	virtual ~RotatingObject();

	virtual void Update(float DeltaTime);

	void SetRotationAxis(const DX XMFLOAT3& Axis);
	void SetAngularSpeed(float AngularSpeed);

protected:
	DX XMFLOAT3 m_RotationAxis;
	float		m_AngularSpeed;
};

_3DGP_END_

