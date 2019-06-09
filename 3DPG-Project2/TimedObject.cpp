#include "stdafx.h"
#include "TimedObject.h"

_3DGP_USE_

TimedObject::TimedObject()
{
}


TimedObject::~TimedObject()
{
}

void TimedObject::Update(float DeltaTime)
{
	m_Lifetime += DeltaTime;
}
