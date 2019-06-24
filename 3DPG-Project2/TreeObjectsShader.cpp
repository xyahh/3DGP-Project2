#include "stdafx.h"
#include "TreeObjectsShader.h"
#include "GameObject.h"
#include "OBJMesh.h"

_3DGP_USE_
DX_USE

TreeObjectsShader::TreeObjectsShader(HeightMapTerrain** pTerrain)
{
	m_Terrain = pTerrain;
}

TreeObjectsShader::~TreeObjectsShader()
{
}

void TreeObjectsShader::BuildObjects(ID3D12Device * pDevice, ID3D12GraphicsCommandList * pCommandList)
{
	Mesh* pLeavesMesh = new OBJMesh(pDevice, pCommandList, "tree_leaves.obj", XMFLOAT4(0.f, 0.5f, 0.f, 1.f));
	Mesh* pTrunkMesh = new OBJMesh(pDevice, pCommandList, "tree_trunk.obj", XMFLOAT4(0.8f, 0.4f, 0.1f, 1.f));

	float DistanceBetweenTrees = 50.f;
	
	float TerrainWidth = static_cast<float>((*m_Terrain)->GetHeightMapWidth());
	float TerrainDepth = static_cast<float>((*m_Terrain)->GetHeightMapDepth());

	UINT ObjectRow = (int)((TerrainWidth / DistanceBetweenTrees) + 1);
	DX XMFLOAT3 Scale = (*m_Terrain)->GetScale();
	DX XMFLOAT3 Offset = (*m_Terrain)->GetOffset();

	m_ObjectCount = ObjectRow  * (UINT)((TerrainDepth / DistanceBetweenTrees) + 1);
	m_Objects.reserve(m_ObjectCount);

	auto GenerateRandomCoordinate = [&](int r)->DX XMFLOAT3
	{
		float X = (r % (int)ObjectRow) * DistanceBetweenTrees * Scale.x;
		float Z = (r / (int)ObjectRow) * DistanceBetweenTrees * Scale.z;
		return DX XMFLOAT3(X , (*m_Terrain)->GetHeight(X, Z), Z);
	};


	GameObject *pObject = NULL;
	for (UINT i = 0; i < m_ObjectCount; ++i)
	{
		pObject = new GameObject;
		DX XMFLOAT3 Pos = GenerateRandomCoordinate(i);

		if (Equal(Pos.y, 0.f))
			delete pObject;
		else
		{
			XMFLOAT4 Quat = GetLookAtQuat(gWorldUp, XMLoadFloat3(&(*m_Terrain)->GetNormal(Pos.x, Pos.z)));
			pObject->Rotate(Quat);
			pObject->Rotate(0.f, 180.f* ((float)rand() / (float)(RAND_MAX)), 0.f);

			pObject->SetPosition(DX XMFLOAT3(Pos.x + Offset.x, Pos.y + Offset.y, Pos.z + Offset.z));
			m_Objects.emplace_back(pObject);
		}
		
	}
	m_Objects[0]->AddMesh(pTrunkMesh);
	m_Objects[0]->AddMesh(pLeavesMesh);
	CreateShaderVariables(pDevice, pCommandList);

}
