#pragma once
#include "Player.h"
#include "TimedObject.h"

_3DGP_BEGIN_

class WagonPlayer : public Player
{
public:
	WagonPlayer(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList, ID3D12RootSignature* pGraphicsRootSignature);
	virtual ~WagonPlayer();

	void SetSubMesh(Mesh* pMesh);
	void SetRailMesh(Mesh* pMesh);

	virtual Camera* ChangeCamera(Camera::MODE NewCameraMode, float DeltaTime);

	virtual void Update(float DeltaTime);
	virtual void Render(ID3D12GraphicsCommandList* pCommandList, Camera* pCamera);
	virtual bool ProcessInput();


protected:

	Mesh*					m_RailMesh		{ NULL };
	Mesh*					m_SubWagonMesh	{ NULL };
	STD list<TimedObject>	m_Rails;
};

_3DGP_END_

