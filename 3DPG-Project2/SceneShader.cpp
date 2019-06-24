#include "stdafx.h"
#include "SceneShader.h"
#include "OBJMesh.h"

_3DGP_USE_
DX_USE

SceneShader::SceneShader()
{
}

SceneShader::~SceneShader()
{
	if (m_Terrain)
		m_Terrain->Release();
}

Player* SceneShader::GetActivePlayer()
{
	return &m_Wagons[0];
}

void SceneShader::MoveWagonsForward(float Distance, RailObjectShader* pRailObjectShader)
{
	for (auto& p : m_Wagons)
	{
		p.MoveForward(Distance);
		pRailObjectShader->AdjustPlayerPosition(&p);
	}
}

void SceneShader::BuildObjects(ID3D12Device * pDevice, ID3D12GraphicsCommandList * pCommandList)
{
	
	//Wagon Build
	{
		Mesh* MainWagon = new OBJMesh(pDevice, pCommandList, "Wagon1.obj", XMFLOAT3(75.f, 75.f, -75.f));
		Mesh* SubWagon = new OBJMesh(pDevice, pCommandList, "Wagon2.obj", XMFLOAT3(75.f, 75.f, -75.f));

		UINT WagonCount = 5;

		m_Wagons.reserve(WagonCount);

		for (int i = 0; i < WagonCount; ++i)
		{
			m_Wagons.emplace_back();
			m_Wagons[i].AddMesh((i == 0) ? MainWagon : SubWagon);
			m_Wagons[i].ChangeCamera(Camera::MODE::ORBITAL, 0.f);
			m_Wagons[i].CreateShaderVariables(pDevice, pCommandList);
			m_Wagons[i].SetPosition(XMFLOAT3(0.f, 0.f, i*-100.f));
			m_Wagons[i].SetWagonNumber(i);
		}
	}
	
	//Terrain Build
	{
		XMFLOAT3 Scale(8.f, 0.f, 8.f);
		XMFLOAT4 Color(0.f, 0.2f, 0.f, 0.f);
		m_Terrain = new HeightMapTerrain(pDevice, pCommandList, "heightmap.raw",
			257, 257, 17, 17, Scale, Color);
	}
}

void SceneShader::ReleaseUploadBuffers()
{
	for (auto& p : m_Wagons)
		p.ReleaseUploadBuffers();

	if (m_Terrain)
		m_Terrain->ReleaseUploadBuffers();
}

void SceneShader::Update(float DeltaTime)
{
	/* Updates */
	for (auto& p : m_Wagons)
		p.Update(DeltaTime);
}

void SceneShader::ReleaseObjects()
{
	if (m_Terrain)
		m_Terrain->Release();
	m_Terrain = NULL;

	m_Wagons.clear();
}

void SceneShader::ReleaseShaderVariables()
{
	for (auto& p : m_Wagons)
		p.ReleaseShaderVariables();

	if (m_Terrain)
		m_Terrain->ReleaseShaderVariables();
}

void SceneShader::Render(ID3D12GraphicsCommandList * pCommandList, Camera * pCamera, float Interpolation)
{
	ObjectsShader::Render(pCommandList, pCamera, Interpolation);
	for (auto& p : m_Wagons)
		p.Render(pCommandList, pCamera);
	if (m_Terrain)
		m_Terrain->Render(pCommandList, pCamera);
}
