#include "stdafx.h"
#include "TimedObject.h"

_3DGP_USE_

TimedObject::TimedObject()
{
}


TimedObject::~TimedObject()
{
}

void TimedObject::SetLifetime(float Lifetime)
{
	m_TotalLifetime = Lifetime;
}

void TimedObject::Update(float DeltaTime)
{
	m_Lifetime += DeltaTime;
}
