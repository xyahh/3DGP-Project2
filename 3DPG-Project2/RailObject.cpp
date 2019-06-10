#include "stdafx.h"
#include "RailObject.h"

_3DGP_USE_

RailObject::RailObject()
{
}


RailObject::~RailObject()
{
}

void RailObject::SetLifetime(float Lifetime)
{
	m_TotalLifetime = Lifetime;
}

void RailObject::SetBehindWagon(int WagonID)
{
	m_BehindWagonID = WagonID;
}

void RailObject::Update(float DeltaTime)
{
	m_Lifetime += DeltaTime;
}
