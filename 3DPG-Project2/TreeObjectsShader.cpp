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
	// 나무 쓰는 Mesh들을 생성
	Mesh* pLeavesMesh = new OBJMesh(pDevice, pCommandList, "tree_leaves.obj", XMFLOAT4(0.f, 0.5f, 0.f, 1.f));
	Mesh* pTrunkMesh = new OBJMesh(pDevice, pCommandList, "tree_trunk.obj", XMFLOAT4(0.8f, 0.4f, 0.1f, 1.f));

	//나무 사이 거리
	float DistanceBetweenTrees = 50.f;
	
	//맵에 존재하는 Terrain의 정보 읽어오기
	float TerrainWidth = static_cast<float>((*m_Terrain)->GetHeightMapWidth());
	float TerrainDepth = static_cast<float>((*m_Terrain)->GetHeightMapDepth());

	UINT ObjectRow = (int)((TerrainWidth / DistanceBetweenTrees) + 1);
	DX XMFLOAT3 Scale = (*m_Terrain)->GetScale();
	DX XMFLOAT3 Offset = (*m_Terrain)->GetOffset();

	//나무 최대 갯수 계산해서 Memory Allocation 하기
	m_ObjectCount = ObjectRow  * (UINT)((TerrainDepth / DistanceBetweenTrees) + 1);
	m_Objects.reserve(m_ObjectCount);

	//Terrain따라서 나무 위치를 계산하기
	auto GetNewTreePosition = [&](int r)->DX XMFLOAT3
	{
		float X = (r % (int)ObjectRow) * DistanceBetweenTrees * Scale.x;
		float Z = (r / (int)ObjectRow) * DistanceBetweenTrees * Scale.z;
		return DX XMFLOAT3(X , (*m_Terrain)->GetHeight(X, Z), Z);
	};


	GameObject *pObject = NULL;
	for (UINT i = 0; i < m_ObjectCount; ++i)
	{
		pObject = new GameObject;
		DX XMFLOAT3 Pos = GetNewTreePosition(i);

		if (Equal(Pos.y, 0.f))
			delete pObject;
		else
		{
			//Terrain의 법선 벡터 따라서 Quaternion Rotation 다릅니다.
			XMFLOAT4 Quat = GetLookRotationQuaternion(gWorldUp, XMLoadFloat3(&(*m_Terrain)->GetNormal(Pos.x, Pos.z)));
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
