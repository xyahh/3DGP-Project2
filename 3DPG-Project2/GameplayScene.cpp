#include "stdafx.h"
#include "GameplayScene.h"
#include "GameFramework.h"

#include "DiffusedShader.h"
#include "RailObjectShader.h"
#include "CubeObjectShader.h"
#include "TreeObjectsShader.h"

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

	m_ObjectShaders.reserve(2);

	m_RailObjectShader = new RailObjectShader;
	m_SceneShader = new SceneShader;

	m_ObjectShaders.emplace_back(m_RailObjectShader);
	m_ObjectShaders.emplace_back(m_SceneShader);
	m_ObjectShaders.emplace_back(new CubeObjectShader);
	m_ObjectShaders.emplace_back(new TreeObjectsShader(m_SceneShader->GetTerrain()));
	for (auto& p : m_ObjectShaders)
	{
		p->CreateShader(pDevice, m_RootSignature.Get());
		p->BuildObjects(pDevice, pCommandList);
	}
	
	return m_SceneShader->GetActivePlayer();
}

void GameplayScene::Destroy()
{
	for (auto& objShaders : m_ObjectShaders)
	{
		objShaders->ReleaseShaderVariables();
		objShaders->ReleaseObjects();
		delete objShaders;
	}
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
	Player* pPlayer = m_SceneShader->GetActivePlayer();

	if (::GetKeyboardState(pKeyBuffer))
	{
		KEY_PRESSED(pKeyBuffer, VK_F1)
			pPlayer->ChangeCamera(Camera::MODE::FIRST_PERSON, Timer::GetDeltaTime());
		KEY_PRESSED(pKeyBuffer, VK_F2)
			pPlayer->ChangeCamera(Camera::MODE::ORBITAL, Timer::GetDeltaTime());
		KEY_PRESSED(pKeyBuffer, VK_F3)
			pPlayer->ChangeCamera(Camera::MODE::THIRD_PERSON, Timer::GetDeltaTime());
		KEY_PRESSED(pKeyBuffer, VK_F4)
			pPlayer->ChangeCamera(Camera::MODE::FIXED, Timer::GetDeltaTime());


		KEY_PRESSED(pKeyBuffer, 'W')
			Rotation.x = 1.f * RotationScale;

		KEY_PRESSED(pKeyBuffer, 'S')
			Rotation.x = -1.f* RotationScale;

		KEY_PRESSED(pKeyBuffer, 'A')
			Rotation.y = -1.f* RotationScale;

		KEY_PRESSED(pKeyBuffer, 'D')
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

	m_RailObjectShader->SetSpawnOrientation(Rotation);

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
			Camera* pCamera = pPlayer->GetCamera();
			KEY_PRESSED(pKeyBuffer, VK_LBUTTON)
				pCamera->Rotate(cyDelta, cxDelta, 0.f);
			KEY_PRESSED(pKeyBuffer, VK_RBUTTON)
				pCamera->Rotate(0.f, 0.f, cxDelta);
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
		objShaders->Update(DeltaTime);

	m_SceneShader->MoveWagonsForward(DeltaTime * BLOCK_LENGTH * RAILS_PER_SEC, m_RailObjectShader);
}

void GameplayScene::ReleaseUploadBuffers()
{
	for (auto& objShaders : m_ObjectShaders)
		objShaders->ReleaseUploadBuffers();
}
 