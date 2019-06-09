#include "stdafx.h"
#include "GameplayScene.h"
#include "DiffusedShader.h"

#include "GameFramework.h"
#include "WagonPlayer.h"
#include "RotatingObject.h"

_3DGP_USE_
DX_USE

GameplayScene::GameplayScene()
{
}

GameplayScene::~GameplayScene()
{
}

Player* GameplayScene::Init(ID3D12Device * pDevice, ID3D12GraphicsCommandList* pCommandList)
{
	m_RootSignature = CreateRootSignature(pDevice);

	m_Player = new WagonPlayer(pDevice, pCommandList, m_RootSignature.Get());

	m_ObjectShaders.reserve(1);

	m_ObjectShaders.emplace_back();
	m_ObjectShaders[0].CreateShader(pDevice, m_RootSignature.Get());
	m_ObjectShaders[0].BuildObjects(pDevice, pCommandList);

	return m_Player;
}

void GameplayScene::Destroy()
{
	for (auto& objShaders : m_ObjectShaders)
	{
		objShaders.ReleaseShaderVariables();
		objShaders.ReleaseObjects();
	}
	m_Player->ReleaseShaderVariables();
	delete m_Player;
}

bool GameplayScene::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		::SetCapture(hWnd);
		::GetCursorPos(&m_CursorPos);
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		::ReleaseCapture();
		break;
	default:
		return false;
	}
	return true;
}

bool GameplayScene::ProcessInput()
{
	if (m_Player && m_Player->ProcessInput())
		return true;
	return false;
}

void GameplayScene::Render(ID3D12GraphicsCommandList * pCommandList, Camera* pCamera, float Interpolation)
{
	pCamera->UpdateViewportsAndScissorRects(pCommandList);
	pCommandList->SetGraphicsRootSignature(m_RootSignature.Get());
	pCamera->UpdateShaderVariables(pCommandList);

	for (auto& objShaders : m_ObjectShaders)
		objShaders.Render(pCommandList, pCamera, Interpolation);
	if (m_Player) m_Player->Render(pCommandList, pCamera);
}

void GameplayScene::Update(float DeltaTime)
{
	/* Time Dilation Interpolation */
	if (!Equal(m_CurrentTimeDilation, m_TargetTimeDilation))
	{
		m_CurrentTimeDilation += (m_TargetTimeDilation - m_CurrentTimeDilation) / 10.f;
		if (Equal(m_CurrentTimeDilation, m_TargetTimeDilation))
			m_CurrentTimeDilation = m_TargetTimeDilation;
	}
	DeltaTime *= m_CurrentTimeDilation;

	for (auto& objShaders : m_ObjectShaders)
		objShaders.Update(DeltaTime);
	if (m_Player) m_Player->Update(DeltaTime);
}

void GameplayScene::ReleaseUploadBuffers()
{
	for (auto& objShaders : m_ObjectShaders)
		objShaders.ReleaseUploadBuffers();
}
 