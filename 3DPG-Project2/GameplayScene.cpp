#include "stdafx.h"
#include "GameplayScene.h"

#include "RotatingObject.h"
#include "DiffusedCubeMesh.h"
#include "DiffusedShader.h"

_3DGP_USE_
DX_USE

GameplayScene::GameplayScene()
{
}

GameplayScene::~GameplayScene()
{
}

void GameplayScene::Init(ID3D12Device * pDevice, ID3D12GraphicsCommandList* pCommandList)
{
	m_RootSignature = CreateRootSignature(pDevice);

	m_ObjectCount = 1;
	m_Objects = new GameObject*[m_ObjectCount];

	Mesh* pMesh = new DiffusedCubeMesh(pDevice, pCommandList, 12.f, 12.f, 12.f);

	{
		DiffusedShader* pDiffShader = new DiffusedShader;
		pDiffShader->CreateShader(pDevice, m_RootSignature);
		pDiffShader->CreateShaderVariables(pDevice, pCommandList);
		
		RotatingObject* pRotatingObject = new RotatingObject;
		pRotatingObject->SetMesh(pMesh);
		pRotatingObject->SetShader(pDiffShader);

		m_Objects[0] = pRotatingObject;
	}

}

void GameplayScene::Destroy()
{
	m_RootSignature->Release();
	if (m_Objects)
	{
		for (int i = 0; i < m_ObjectCount; ++i)
		{
			if (m_Objects[i]) delete m_Objects[i];
		}
		delete[] m_Objects;
	}
}

bool GameplayScene::MouseMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return false;
}

bool GameplayScene::KeyboardMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return false;
}

void GameplayScene::Render(ID3D12GraphicsCommandList * pCommandList, Camera* pCamera, float Interpolation)
{
	pCamera->UpdateViewportsAndScissorRects(pCommandList);
	pCommandList->SetGraphicsRootSignature(m_RootSignature);
	pCamera->UpdateShaderVariables(pCommandList);
	for (int i = 0; i < m_ObjectCount; ++i)
	{
		if (m_Objects[i]) m_Objects[i]->Render(pCommandList, pCamera);
	}
}

void GameplayScene::Update(float DeltaTime)
{
	for (int i = 0; i < m_ObjectCount; ++i)
	{
		if (m_Objects[i]) m_Objects[i]->Update(DeltaTime);
	}
}

void GameplayScene::ReleaseUploadBuffers()
{
	if (m_Objects)
	{
		for (int i = 0; i < m_ObjectCount; ++i)
			if (m_Objects[i]) m_Objects[i]->ReleaseUploadBuffers();
	}
}
 