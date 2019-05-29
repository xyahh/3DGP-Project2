#include "stdafx.h"
#include "GameplayScene.h"

_3DGP_USE_

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

	//Shader 0
	{
		Shader* pShader = new Shader;
		pShader->CreateShader(pDevice, m_RootSignature.Get());
		pShader->BuildObjects(pDevice, pCommandList, NULL);

		m_Shaders[0] = pShader;
		m_Shaders[0]->AddRef();
	}
}

void GameplayScene::Destroy()
{
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
	pCommandList->SetGraphicsRootSignature(m_RootSignature.Get());

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
 