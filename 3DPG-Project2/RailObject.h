#pragma once
#include "GameObject.h"

_3DGP_BEGIN_

class RailObject : public GameObject
{
public:
	RailObject();
	~RailObject();

	void SetLifetime(float Lifetime);
	void SetBehindWagon(int WagonID);
	int GetBehindWagonID() const { return m_BehindWagonID; }

	virtual void Update(float DeltaTime);
	bool IsExpired() const { return m_Lifetime >= m_TotalLifetime; };


private:

	int		m_BehindWagonID	{ -1 };
	float	m_Lifetime		{ 0.f };
	float	m_TotalLifetime	{ 10.f };
};

_3DGP_END_

