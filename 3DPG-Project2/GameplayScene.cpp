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

	m_ShaderCount = 1;
	m_Shaders = new ObjectsShader[m_ShaderCount];

	m_Shaders[0].CreateShader(pDevice, m_RootSignature);
	m_Shaders[0].BuildObjects(pDevice, pCommandList);
}

void GameplayScene::Destroy()
{
	m_RootSignature->Release();
	if (m_Shaders)
	{
		for (int i = 0; i < m_ShaderCount; ++i)
		{
			m_Shaders[i].ReleaseShaderVariables();
			m_Shaders[i].ReleaseObjects();
		}
		delete[] m_Shaders;
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

	for (int i = 0; i < m_ShaderCount; ++i)
		m_Shaders[i].Render(pCommandList, pCamera, Interpolation);
}

void GameplayScene::Update(float DeltaTime)
{
	for (int i = 0; i < m_ShaderCount; ++i)
		m_Shaders[i].Update(DeltaTime);
}

void GameplayScene::ReleaseUploadBuffers()
{
	if (m_Shaders)
	{
		for (int i = 0; i < m_ShaderCount; ++i)
			m_Shaders[i].ReleaseUploadBuffers();
	}
}
 