#include "stdafx.h"
#include "RailObjectShader.h"
#include "GameObject.h"
#include "RailObject.h"
#include "OBJMesh.h"

_3DGP_USE_
DX_USE

RailObjectShader::RailObjectShader()
	: m_SpawnOrientation(0.f, 0.f, 0.f)
{
}

RailObjectShader::~RailObjectShader()
{
}

void RailObjectShader::BuildObjects(ID3D12Device * pDevice, ID3D12GraphicsCommandList * pCommandList)
{
	m_RailMesh = new OBJMesh(pDevice, pCommandList, "Rail.obj", XMFLOAT3(BLOCK_LENGTH, BLOCK_LENGTH, BLOCK_LENGTH));
	m_ObjectCount = static_cast<UINT>(RAIL_LIFETIME / RAIL_SPAWN_RATE); //Make sure that the vector capacity is big enough for all the Rails to spawn/despawn
	m_RailObjects.reserve(static_cast<size_t>(m_ObjectCount));

	CreateShaderVariables(pDevice, pCommandList);
}

void RailObjectShader::ReleaseObjects()
{
	ObjectShader::ReleaseObjects();
	m_RailObjects.clear();
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

void RailObjectShader::SetSpawnOrientation(const DX XMFLOAT3 & Orientation)
{
	m_SpawnOrientation = Orientation;
}

void RailObjectShader::UpdateShaderVariables(ID3D12GraphicsCommandList * pCommandList)
{
	for (int i = 0; i < m_RailObjects.size(); ++i)
		XMStoreFloat4x4(&m_MappedGameObjects[i].m_Transform, XMMatrixTranspose(XMLoadFloat4x4(&m_RailObjects[i].GetWorldTransform())));
}

void RailObjectShader::Update(float DeltaTime)
{
	m_SpawnTimer += DeltaTime;
	if (m_SpawnTimer > RAIL_SPAWN_RATE)
	{
		m_SpawnTimer -= RAIL_SPAWN_RATE;
		SpawnRail();
	}

	for (auto& i : m_RailObjects)
		i.Update(DeltaTime);

	/* Remove Expired Rails */
	static auto RemoveCondition = [](const RailObject& rail) { return rail.IsExpired();	};
	m_RailObjects.erase(STD remove_if(m_RailObjects.begin(), m_RailObjects.end(), RemoveCondition), m_RailObjects.end());
}

void RailObjectShader::Render(ID3D12GraphicsCommandList * pCommandList, Camera * pCamera, float Interpolation)
{
	Shader::Render(pCommandList, pCamera);
	UpdateShaderVariables(pCommandList);
	m_RailMesh->Render(pCommandList, m_ObjectCount, m_InstancingBufferView);
}

void RailObjectShader::SpawnRail()
{
	RailObject RailObject;
	RailObject.SetLifetime(RAIL_LIFETIME);

	if (m_RailObjects.size() > 0)
	{
		RailObject.SetWorldTransform(m_RailObjects.back().GetWorldTransform());
		RailObject.MoveForward(BLOCK_LENGTH);
		RailObject.Rotate(m_SpawnOrientation.x, m_SpawnOrientation.y, m_SpawnOrientation.z);

		m_RailObjects.push_back(std::move(RailObject));
	}
	else
	{
		RailObject.SetPosition(XMVectorZero());
		/*Start with 20 blocks*/
		for (int i = 0; i < 20; ++i)
		{
			RailObject.SetLifetime(RAIL_LIFETIME - (20 - i) * RAIL_SPAWN_RATE);
			m_RailObjects.push_back(RailObject);
			RailObject.MoveForward(BLOCK_LENGTH);
		}

		
	}

	
}


