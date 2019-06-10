﻿#include "stdafx.h"
#include "GameplayScene.h"
#include "GameFramework.h"

#include "RailObjectShader.h"
#include "WagonPlayer.h"
#include "OBJMesh.h"

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

	Mesh* MainWagon = new OBJMesh(pDevice, pCommandList, "Wagon1.obj", XMFLOAT3(75.f, 75.f, -75.f));
	Mesh* SubWagon =  new OBJMesh(pDevice, pCommandList, "Wagon2.obj", XMFLOAT3(75.f, 75.f, -75.f));

	m_Wagons.reserve(5);

	m_Wagons.emplace_back(pDevice, pCommandList, m_RootSignature.Get());
	m_Wagons[0].SetMesh(MainWagon);
	m_Wagons[0].ChangeCamera(Camera::MODE::THIRD_PERSON, 0.f);
	m_Wagons[0].CreateShaderVariables(pDevice, pCommandList);
	m_Wagons[0].SetPosition(XMFLOAT3(0.f, 0.f, 0.f));
	m_Wagons[0].SetWagonNumber(0);

	for (int i = 1; i < 5; ++i)
	{
		m_Wagons.emplace_back(pDevice, pCommandList, m_RootSignature.Get());
		m_Wagons[i].SetMesh(SubWagon);
		m_Wagons[i].ChangeCamera(Camera::MODE::THIRD_PERSON, 0.f);
		m_Wagons[i].CreateShaderVariables(pDevice, pCommandList);
		m_Wagons[i].SetPosition(XMFLOAT3(0.f, 0.f, i*-100.f));
		m_Wagons[i].SetWagonNumber(i);
	}

	m_ObjectShaders.reserve(1);

	m_pRailObjectShader = new RailObjectShader;
	m_ObjectShaders.emplace_back(m_pRailObjectShader);

	for (auto& p : m_ObjectShaders)
	{
		p->CreateShader(pDevice, m_RootSignature.Get());
		p->BuildObjects(pDevice, pCommandList);
	}
	return &m_Wagons[0];
}

void GameplayScene::Destroy()
{
	for (auto& objShaders : m_ObjectShaders)
	{
		objShaders->ReleaseShaderVariables();
		objShaders->ReleaseObjects();
		delete objShaders;
	}
	for(auto& p : m_Wagons)
		p.ReleaseShaderVariables();
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

void GameplayScene::ProcessInput()
{
	//return;
		
	static UCHAR pKeyBuffer[256];
	DWORD dwDirection = 0;
	if (::GetKeyboardState(pKeyBuffer))
	{
		KEY_PRESSED(pKeyBuffer, 'A')
		{
			m_pRailObjectShader->SetSpawnRotation(XMFLOAT3(1.f, 0.f, 0.f));
		}
	}

	float cxDelta = 0.0f, cyDelta = 0.0f;

	static HWND hWnd = GameFramework::Get()->GetWindowHandle();

	POINT ptCursorPos;
	if (::GetCapture() == hWnd)
	{
		::SetCursor(NULL);
		::GetCursorPos(&ptCursorPos);
		cxDelta = (float)(ptCursorPos.x - m_CursorPos.x);
		cyDelta = (float)(ptCursorPos.y - m_CursorPos.y);
		::SetCursorPos(m_CursorPos.x, m_CursorPos.y);
	}
	if ((cxDelta != 0.0f) || (cyDelta != 0.0f))
	{
		if (cxDelta || cyDelta)
		{
			if (pKeyBuffer[VK_RBUTTON] & 0xF0)
				m_Wagons[0].GetCamera()->Rotate(cyDelta, 0.0f, -cxDelta);
			else
				m_Wagons[0].GetCamera()->Rotate(cyDelta, cxDelta, 0.0f);
		}
	}
}

void GameplayScene::Render(ID3D12GraphicsCommandList * pCommandList, Camera* pCamera, float Interpolation)
{
	pCamera->UpdateViewportsAndScissorRects(pCommandList);
	pCommandList->SetGraphicsRootSignature(m_RootSignature.Get());
	pCamera->UpdateShaderVariables(pCommandList);

	for (auto& objShaders : m_ObjectShaders)
		objShaders->Render(pCommandList, pCamera, Interpolation);

	for (auto& p : m_Wagons)
		p.Render(pCommandList, pCamera);
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

	/* Updates */
	for (auto& p : m_Wagons)
		p.Update(DeltaTime);

	for (auto& objShaders : m_ObjectShaders)
		objShaders->Update(DeltaTime);

	/* Move Forward */
	float MoveDistance = DeltaTime * BLOCK_LENGTH * RAILS_PER_SEC;
	for (auto& p : m_Wagons)
	{
		p.MoveForward(MoveDistance);
		m_pRailObjectShader->AdjustPlayerPosition(&p);
	}
}

void GameplayScene::ReleaseUploadBuffers()
{
	for (auto& objShaders : m_ObjectShaders)
		objShaders->ReleaseUploadBuffers();
}
 