#include "stdafx.h"
#include "RotatingObject.h"


_3DGP_USE_

RotatingObject::RotatingObject() 
	: m_RotationAxis(0.f, 1.f, 0.f)
	, m_AngularSpeed(90.f)
{
}

RotatingObject::~RotatingObject()
{
}

void RotatingObject::Update(float DeltaTime)
{
	GameObject::Rotate(&m_RotationAxis, m_AngularSpeed * DeltaTime);
}
