#include "stdafx.h"
#include "GameplayScene.h"
#include "GameFramework.h"

#include "DiffusedShader.h"
#include "RailObjectShader.h"
#include "InstancingShader.h"

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
	Shader* PlayerShader = new DiffusedShader;
	PlayerShader->CreateShader(pDevice, m_RootSignature.Get());

	m_Wagons.reserve(5);

	m_Wagons.emplace_back(pDevice, pCommandList, m_RootSignature.Get());
	m_Wagons[0].SetMesh(MainWagon);
	m_Wagons[0].ChangeCamera(Camera::MODE::THIRD_PERSON, 0.f);
	m_Wagons[0].CreateShaderVariables(pDevice, pCommandList);
	m_Wagons[0].SetPosition(XMFLOAT3(0.f, 0.f, 0.f));
	m_Wagons[0].SetWagonNumber(0);
	m_Wagons[0].SetShader(PlayerShader);

	for (int i = 1; i < 5; ++i)
	{
		m_Wagons.emplace_back(pDevice, pCommandList, m_RootSignature.Get());
		m_Wagons[i].SetMesh(SubWagon);
		m_Wagons[i].SetShader(PlayerShader);
		m_Wagons[i].ChangeCamera(Camera::MODE::THIRD_PERSON, 0.f);
		m_Wagons[i].CreateShaderVariables(pDevice, pCommandList);
		m_Wagons[i].SetPosition(XMFLOAT3(0.f, 0.f, i*-100.f));
		m_Wagons[i].SetWagonNumber(i);
	}

	m_ObjectShaders.reserve(2);

	m_pRailObjectShader = new RailObjectShader;
	m_ObjectShaders.emplace_back(m_pRailObjectShader);
	m_ObjectShaders.emplace_back(new InstancingShader);

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
	static UCHAR pKeyBuffer[256];
	const static float RotationScale{ 3.f };
	XMFLOAT3 Rotation = XMFLOAT3(0.f, 0.f, 0.f);

	if (::GetKeyboardState(pKeyBuffer))
	{
		KEY_PRESSED(pKeyBuffer, VK_UP)
			Rotation.x = 1.f * RotationScale;

		KEY_PRESSED(pKeyBuffer, VK_DOWN)
			Rotation.x = -1.f* RotationScale;

		KEY_PRESSED(pKeyBuffer, VK_LEFT)
			Rotation.y = -1.f* RotationScale;

		KEY_PRESSED(pKeyBuffer, VK_RIGHT)
			Rotation.y = 1.f* RotationScale;

		KEY_PRESSED(pKeyBuffer, 'Q')
			Rotation.z = 1.f* RotationScale; //make roll stronger to feel a bigger difference

		KEY_PRESSED(pKeyBuffer, 'E')
			Rotation.z = -1.f* RotationScale;

		KEY_PRESSED(pKeyBuffer, VK_SPACE)
			m_TargetTimeDilation = 0.1f; //slow down time by 10
		else
			m_TargetTimeDilation = 1.f;
	}

	m_pRailObjectShader->SetSpawnOrientation(Rotation);

	float cxDelta = 0.0f, cyDelta = 0.0f;

	static HWND hWnd = GameFramework::Get()->GetWindowHandle();

	POINT ptCursorPos;
	if (::GetCapture() == hWnd)
	{
		::SetCursor(NULL);
		::GetCursorPos(&ptCursorPos);
		cxDelta = (float)(ptCursorPos.x - m_CursorPos.x) / 10.f;
		cyDelta = (float)(ptCursorPos.y - m_CursorPos.y) / 10.f;
		::SetCursorPos(m_CursorPos.x, m_CursorPos.y);
	}
	if ((cxDelta != 0.0f) || (cyDelta != 0.0f))
	{
		if (cxDelta || cyDelta)
		{
			KEY_PRESSED(pKeyBuffer, VK_LBUTTON)
				m_Wagons[0].GetCamera()->Rotate(cyDelta, cxDelta, 0.f);
			KEY_PRESSED(pKeyBuffer, VK_RBUTTON)
				m_Wagons[0].GetCamera()->Rotate(0.f, 0.f, cxDelta);
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
 