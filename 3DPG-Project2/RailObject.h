#pragma once
#include "TimedObject.h"

_3DGP_BEGIN_

class RailObject : public TimedObject
{
public:
	RailObject();
	virtual ~RailObject();

	
	void SetBehindWagon(int WagonID);
	int GetBehindWagonID() const { return m_BehindWagonID; }

private:

	int		m_BehindWagonID	{ -1 };
};

_3DGP_END_

