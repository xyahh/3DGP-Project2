#pragma once
#include "GameObject.h"

_3DGP_BEGIN_

class TimedObject : public GameObject
{
public:
	TimedObject();
	~TimedObject();

	void SetLifetime(float Lifetime);
	void SetBehindWagon(int WagonID);

	virtual void Update(float DeltaTime);
	bool IsExpired() const { return m_Lifetime >= m_TotalLifetime; };


private:

	int		m_BehindWagonID	{ -1 };
	float	m_Lifetime		{ 0.f };
	float	m_TotalLifetime	{ 10.f };
};

_3DGP_END_

