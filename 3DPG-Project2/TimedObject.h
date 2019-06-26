#pragma once

_3DGP_BEGIN_

class TimedObject : public GameObject
{
public:
	TimedObject();
	virtual ~TimedObject();

	void SetLifetime(float Lifetime);

	virtual void Update(float DeltaTime);
	bool IsExpired() const { return m_Lifetime >= m_TotalLifetime; };

protected:

	float	m_Lifetime{ 0.f };
	float	m_TotalLifetime{ 10.f };

};

_3DGP_END_