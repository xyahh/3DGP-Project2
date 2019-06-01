#include "stdafx.h"
#include "RotatingObject.h"


_3DGP_USE_

RotatingObject::RotatingObject() 
	: m_RotationAxis(0.f, 0.f, 0.f)
	, m_AngularSpeed(0.f)
{
}

RotatingObject::~RotatingObject()
{
}

void RotatingObject::Update(float DeltaTime)
{
	GameObject::Rotate(m_RotationAxis, m_AngularSpeed * DeltaTime);
}

void RotatingObject::SetRotationAxis(const DX XMFLOAT3 & Axis)
{
	m_RotationAxis = Axis;
}

void RotatingObject::SetAngularSpeed(float AngularSpeed)
{
	m_AngularSpeed = AngularSpeed;
}
