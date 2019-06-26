#include "stdafx.h"
#include "RailObjectShader.h"
#include "GameObject.h"
#include "RailObject.h"
#include "OBJMesh.h"

_3DGP_USE_
DX_USE

RailObjectShader::RailObjectShader(HeightMapTerrain** ppTerrain)
	: m_SpawnOrientation(0.f, 0.f, 0.f)
{
	m_Terrain = ppTerrain;
}

RailObjectShader::~RailObjectShader()
{
	if (m_RailMesh) m_RailMesh->Release();
	if (m_ColumnA) m_ColumnA->Release();
	if (m_ColumnB) m_ColumnB->Release();
}

void RailObjectShader::BuildObjects(ID3D12Device * pDevice, ID3D12GraphicsCommandList * pCommandList)
{
	DX XMFLOAT3 Scale (BLOCK_LENGTH, BLOCK_LENGTH, BLOCK_LENGTH);

	m_RailMesh = new OBJMesh(pDevice, pCommandList, "Rail.obj"		, DX XMFLOAT4(0.75f, 0.10f, 0.1f, 1.f), Scale);
	m_ColumnA = new OBJMesh(pDevice, pCommandList, "ColumnPartA.obj", DX XMFLOAT4(0.75f, 0.75f, 0.1f, 1.f), Scale);

	Scale.y = 1.f;
	m_ColumnB = new OBJMesh(pDevice, pCommandList, "ColumnPartB.obj", DX XMFLOAT4(0.75f, 0.75f, 0.1f, 1.f), Scale);

	/* In case there are no objects left to store the Mesh.
	Prevent from automatically being deleted*/
	if (m_RailMesh) m_RailMesh->AddRef();
	if (m_ColumnA) m_ColumnA->AddRef();
	if (m_ColumnB) m_ColumnB->AddRef();

	size_t ObjectCount = static_cast<size_t>(RAIL_LIFETIME / RAIL_SPAWN_RATE); //Make sure that the vector capacity is big enough for all the Rails to spawn/despawn
	m_RailObjects.reserve(ObjectCount);
	m_ColumnObjects.reserve((ObjectCount / (size_t)RAILS_PER_COLUMN) + 1);
	CreateShaderVariables(pDevice, pCommandList);
}

void RailObjectShader::ReleaseObjects()
{
	ObjectsShader::ReleaseObjects();
	m_RailObjects.clear();
	m_ColumnObjects.clear();
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

inline void RailObjectShader::CreateShaderVariables(ID3D12Device * pDevice, ID3D12GraphicsCommandList * pCommandList)
{
	UINT Stride = sizeof(VS_VB_INSTANCE);
	UINT RailCount = (UINT)m_RailObjects.capacity();
	UINT ColumnCount = (UINT)m_ColumnObjects.capacity();

	//Rail Meshes
	{
		
		m_ObjectsCB = CreateBufferResource(pDevice, pCommandList, NULL
			, Stride * RailCount
			, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

		m_ObjectsCB->Map(0, NULL, (void**)&m_MappedGameObjects);

		m_InstancingBufferView.BufferLocation = m_ObjectsCB->GetGPUVirtualAddress();
		m_InstancingBufferView.StrideInBytes = Stride;
		m_InstancingBufferView.SizeInBytes = Stride * RailCount;
	}
	
	//Column A
	{
	
		m_ColumnAResource = CreateBufferResource(pDevice, pCommandList, NULL
			, Stride * ColumnCount
			, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

		m_ColumnAResource->Map(0, NULL, (void**)&m_ColumnAMapping);

		m_ColumnAInstancingBufferView.BufferLocation = m_ColumnAResource->GetGPUVirtualAddress();
		m_ColumnAInstancingBufferView.StrideInBytes = Stride;
		m_ColumnAInstancingBufferView.SizeInBytes = Stride * ColumnCount;
	}

	//Column B
	{
		m_ColumnBResource = CreateBufferResource(pDevice, pCommandList, NULL
			, Stride * ColumnCount
			, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

		m_ColumnBResource->Map(0, NULL, (void**)&m_ColumnBMapping);

		m_ColumnBInstancingBufferView.BufferLocation = m_ColumnBResource->GetGPUVirtualAddress();
		m_ColumnBInstancingBufferView.StrideInBytes = Stride;
		m_ColumnBInstancingBufferView.SizeInBytes = Stride * ColumnCount;
	}
	

}

void RailObjectShader::UpdateShaderVariables(ID3D12GraphicsCommandList * pCommandList)
{

	/* Update Shader Variables for RailMesh */
	for (int i = 0; i < m_RailObjects.size(); ++i)
		XMStoreFloat4x4(&m_MappedGameObjects[i].m_Transform, XMMatrixTranspose(m_RailObjects[i].GetWorldTransformMatrix()));

	/* Update Shader Variables for Column A */
	for (int i = 0; i < m_ColumnObjects.size(); ++i)
		XMStoreFloat4x4(&m_ColumnAMapping[i].m_Transform, XMMatrixTranspose(m_ColumnObjects[i].GetWorldTransformMatrix()));

	/* Update Shader Variables for Column B */
	for (int i = 0; i < m_ColumnObjects.size(); ++i)
		XMStoreFloat4x4(&m_ColumnBMapping[i].m_Transform, XMMatrixTranspose(m_ColumnObjects[i].GetColumnWorldTransform()));
}

void RailObjectShader::ReleaseShaderVariables()
{

	InstancingShader::ReleaseShaderVariables();

	if (m_ColumnAResource.Get()) m_ColumnAResource->Unmap(0, NULL);
	m_ColumnAResource.Reset();

	if (m_ColumnBResource.Get()) m_ColumnBResource->Unmap(0, NULL);
	m_ColumnBResource.Reset();

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

	for (auto& i : m_ColumnObjects)
		i.Update(DeltaTime);

	/* Remove Expired Rails */
	static auto RemoveCondition = [](const TimedObject& timedObject) { return timedObject.IsExpired();	};
	
	m_RailObjects.erase(STD remove_if(m_RailObjects.begin(), m_RailObjects.end(), RemoveCondition), m_RailObjects.end());
	m_ColumnObjects.erase(STD remove_if(m_ColumnObjects.begin(), m_ColumnObjects.end(), RemoveCondition), m_ColumnObjects.end());
}

void RailObjectShader::Render(ID3D12GraphicsCommandList * pCommandList, Camera * pCamera, float Interpolation)
{
	Shader::Render(pCommandList, pCamera);
	UpdateShaderVariables(pCommandList);

	m_RailMesh->Render(pCommandList, m_RailObjects.size(), m_InstancingBufferView);
	m_ColumnA->Render(pCommandList, m_ColumnObjects.size(), m_ColumnAInstancingBufferView);
	m_ColumnB->Render(pCommandList, m_ColumnObjects.size(), m_ColumnBInstancingBufferView);
}

void RailObjectShader::SpawnRail()
{
	static int SPAWN_COLUMN = 0;

	RailObject RailObject;
	RailObject.SetLifetime(RAIL_LIFETIME);

	if (m_RailObjects.size() > 0)
	{
		RailObject.SetWorldTransform(m_RailObjects.back().GetWorldTransform());
		RailObject.MoveForward(BLOCK_LENGTH);
		RailObject.Rotate(m_SpawnOrientation.x, m_SpawnOrientation.y, m_SpawnOrientation.z);

		if (++SPAWN_COLUMN >= RAILS_PER_COLUMN) //every number of rails, spawn one Column
		{
			SPAWN_COLUMN = 0;
			SpawnColumn(RailObject);
		}

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

void RailObjectShader::SpawnColumn(const RailObject& ConnectedRail)
{
	if (!m_Terrain) return;


	ColumnObject Column;
	Column.SetWorldTransform(ConnectedRail.GetWorldTransform());
	Column.CalculateColumnTransform(ConnectedRail.GetPosition(), ConnectedRail.GetLookVector(), *m_Terrain);
	m_ColumnObjects.emplace_back(std::move(Column));
}

