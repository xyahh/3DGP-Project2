#include "stdafx.h"
#include "GameplayScene.h"

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
	m_Shaders = new Shader*[m_ShaderCount];

	m_Shaders[0] = new Shader;
	m_Shaders[0]->CreateShader(pDevice, m_RootSignature);
	m_Shaders[0]->BuildObjects(pDevice, pCommandList, NULL);


}

void GameplayScene::Destroy()
{
	m_RootSignature->Release();
	if (m_Shaders)
	{
		for (int i = 0; i < m_ShaderCount; ++i)
		{
			if (m_Shaders[i])
			{
				m_Shaders[i]->ReleaseShaderVariables();
				m_Shaders[i]->ReleaseObjects();
				m_Shaders[i]->Release();
			}
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

void GameplayScene::Render(ID3D12GraphicsCommandList * pCommandList, float Interpolation)
{
	pCommandList->SetGraphicsRootSignature(m_RootSignature);
	for (int i = 0; i < m_ShaderCount; ++i)
	{
		if (m_Shaders[i]) m_Shaders[i]->Render(pCommandList);
	}
}

void GameplayScene::Update(float DeltaTime)
{
	for (int i = 0; i < m_ShaderCount; ++i)
	{
		if (m_Shaders[i]) m_Shaders[i]->Update(DeltaTime);
	}
}

void GameplayScene::ReleaseUploadBuffers()
{
	if (m_Shaders)
	{
		for (int i = 0; i < m_ShaderCount; ++i)
			if (m_Shaders[i]) m_Shaders[i]->ReleaseUploadBuffers();
	}
}
 