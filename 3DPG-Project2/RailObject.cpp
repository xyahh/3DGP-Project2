#include "stdafx.h"
#include "RailObject.h"

_3DGP_USE_

RailObject::RailObject()
{
}


RailObject::~RailObject()
{
}

void RailObject::SetBehindWagon(int WagonID)
{
	m_BehindWagonID = WagonID;
}

