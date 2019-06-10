#include "stdafx.h"
#include "RailObjectShader.h"
#include "GameObject.h"
#include "RailObject.h"
#include "OBJMesh.h"

_3DGP_USE_
DX_USE

RailObjectShader::RailObjectShader()
	: m_SpawnRotation(0.f, 0.f, 0.f)
{
}


RailObjectShader::~RailObjectShader()
{
}

void RailObjectShader::BuildObjects(ID3D12Device * pDevice, ID3D12GraphicsCommandList * pCommandList)
{
	m_RailMesh = new OBJMesh(pDevice, pCommandList, "Rail.obj", XMFLOAT3(BLOCK_LENGTH, BLOCK_LENGTH, BLOCK_LENGTH));
	m_RailObjects.reserve((int)(RAIL_LIFETIME / RAIL_SPAWN_RATE) + 50);
}

void RailObjectShader::ReleaseObjects()
{
	ObjectShader::ReleaseObjects();
	m_RailObjects.clear();
}

void RailObjectShader::ReleaseUploadBuffers()
{
	ObjectShader::ReleaseUploadBuffers();
	for (auto& i : m_RailObjects)
		i.ReleaseUploadBuffers();
}

void RailObjectShader::AdjustPlayerPosition(WagonPlayer * pPlayer)
{
	RailObject* BackRail	{ NULL };
	RailObject* FrontRail	{ NULL };

	int PlayerWagonID = pPlayer->GetWagonNumber();

	for (auto& rail : m_RailObjects)
	{
		if (rail.GetBehindWagonID() >= PlayerWagonID)
		{
			BackRail = &rail; 
		} else  if (SubjectBehindObject(&rail, pPlayer))
		{
			rail.SetBehindWagon(PlayerWagonID);
			BackRail = &rail;//get the latest rail that is behind player before breaking loop
		}
		else
		{
			FrontRail = &rail;
			break; //get the first front rail and break the loop
		}
	}

	if (BackRail && FrontRail)
	{
		float RailOffset = XMVectorGetX(XMVector3Length(BackRail->GetPositionVector() - pPlayer->GetPositionVector())) / BLOCK_LENGTH;;
		//Be 100% sure that Prevrailoffset is in the range [0,1]
		Clamp(0.f, &RailOffset, 1.f);

		//interpolate the transforms to have a smoother transition.
		pPlayer->SetPosition(XMVectorLerp(BackRail->GetPositionVector(), FrontRail->GetPositionVector(), RailOffset));
		pPlayer->SetRight(XMVectorLerp(BackRail->GetRightVector(), FrontRail->GetRightVector(), RailOffset));
		pPlayer->SetUp(XMVectorLerp(BackRail->GetUpVector(), FrontRail->GetUpVector(), RailOffset));
		pPlayer->SetLook(XMVectorLerp(BackRail->GetLookVector(), FrontRail->GetLookVector(), RailOffset));
	}
}

void RailObjectShader::SetSpawnRotation(const DX XMFLOAT3 & Rotation)
{
	m_SpawnRotation = Rotation;
}

void RailObjectShader::Update(float DeltaTime)
{
	ObjectShader::Update(DeltaTime);
	m_SpawnTimer += DeltaTime;
	if (m_SpawnTimer > RAIL_SPAWN_RATE)
	{
		m_SpawnTimer -= RAIL_SPAWN_RATE;
		SpawnRail();
	}

	for (auto& i : m_RailObjects)
		i.Update(DeltaTime);

	for (auto i = m_RailObjects.begin(); i != m_RailObjects.end();)
	{
		if (i->IsExpired()) i = m_RailObjects.erase(i);
		else break; //Stop loop when first encounter an object with lifetime remaining (objects added later have more lifetime remaining)
	}

}

void RailObjectShader::Render(ID3D12GraphicsCommandList * pCommandList, Camera * pCamera, float Interpolation)
{
	ObjectShader::Render(pCommandList, pCamera, Interpolation);
	for (auto& i : m_RailObjects)
		i.Render(pCommandList, pCamera);
}


void RailObjectShader::SpawnRail()
{
	RailObject RailObject;
	RailObject.SetLifetime(RAIL_LIFETIME);

	if (m_RailObjects.size() > 0)
	{
		RailObject.SetWorldTransform(m_RailObjects.back().GetWorldTransform());
		RailObject.MoveForward(BLOCK_LENGTH);
		RailObject.Rotate(m_SpawnRotation.x, m_SpawnRotation.y, m_SpawnRotation.z);

		m_RailObjects.push_back(std::move(RailObject));
		m_RailObjects.back().SetMesh(m_RailMesh);
	}
	else
	{
		RailObject.SetPosition(XMVectorZero());
		for (int i = 0; i < 20; ++i)
		{
			RailObject.SetLifetime(RAIL_LIFETIME + (20 - i) * RAIL_SPAWN_RATE);
			m_RailObjects.push_back(RailObject);
			m_RailObjects.back().SetMesh(m_RailMesh);
			RailObject.MoveForward(BLOCK_LENGTH); //10 blocks ahead
		}

		
	}

	
}


