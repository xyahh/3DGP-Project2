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

	m_Player = new WagonPlayer(pDevice, pCommandList, m_RootSignature);

	m_ObjectShaderCount = 1;
	m_ObjectShaders = new ObjectsShader[m_ObjectShaderCount];
	
	m_ObjectShaders[0].CreateShader(pDevice, m_RootSignature);
	m_ObjectShaders[0].BuildObjects(pDevice, pCommandList);

	return m_Player;
}

void GameplayScene::Destroy()
{
	m_RootSignature->Release();
	if (m_ObjectShaders)
	{
		for (int i = 0; i < m_ObjectShaderCount; ++i)
		{
			m_ObjectShaders[i].ReleaseShaderVariables();
			m_ObjectShaders[i].ReleaseObjects();
		}
		delete[] m_ObjectShaders;
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
		::GetCursorPos(&m_OldCursorPos);
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		::ReleaseCapture();
		break;
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_F1:
			if (m_Player) m_Player->ChangeCamera(Camera::MODE::FIRST_PERSON, Timer::GetDeltaTime());
			break;
		case VK_F2:
			if (m_Player) m_Player->ChangeCamera(Camera::MODE::THIRD_PERSON, Timer::GetDeltaTime());
			break;
		case VK_F3:
			if (m_Player) m_Player->ChangeCamera(Camera::MODE::SPACESHIP, Timer::GetDeltaTime());
			break;
		}
	default:
		return false;
	}
	return true;
}

void GameplayScene::ProcessInput()
{
	static UCHAR pKeyBuffer[256];
	DWORD dwDirection = 0;
	if (::GetKeyboardState(pKeyBuffer))
	{
		if (pKeyBuffer[VK_UP]    & 0xF0)	dwDirection |= DIR_FORWARD;
		if (pKeyBuffer[VK_DOWN]  & 0xF0)	dwDirection |= DIR_BACKWARD;
		if (pKeyBuffer[VK_LEFT]  & 0xF0)	dwDirection |= DIR_LEFT;
		if (pKeyBuffer[VK_RIGHT] & 0xF0)	dwDirection |= DIR_RIGHT;
		if (pKeyBuffer[VK_PRIOR] & 0xF0)	dwDirection |= DIR_UP;
		if (pKeyBuffer[VK_NEXT]  & 0xF0)	dwDirection |= DIR_DOWN;
	}

	float cxDelta = 0.0f, cyDelta = 0.0f;

	static HWND hWnd = GameFramework::Get()->GetWindowHandle();

	POINT ptCursorPos; 
	if (::GetCapture() == hWnd)
	{
		::SetCursor(NULL);
		::GetCursorPos(&ptCursorPos);
		cxDelta = (float)(ptCursorPos.x - m_OldCursorPos.x) / 3.0f;
		cyDelta = (float)(ptCursorPos.y - m_OldCursorPos.y) / 3.0f;
		::SetCursorPos(m_OldCursorPos.x, m_OldCursorPos.y);
	}
	if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f))
	{
		if (cxDelta || cyDelta)
		{
			if (pKeyBuffer[VK_RBUTTON] & 0xF0)
				m_Player->Rotate(cyDelta, 0.0f, -cxDelta);
			else
				m_Player->Rotate(cyDelta, cxDelta, 0.0f);
		}
		if (dwDirection) 
			m_Player->Move(dwDirection, 10.0f * Timer::GetDeltaTime(), true);
	}
}

void GameplayScene::Render(ID3D12GraphicsCommandList * pCommandList, Camera* pCamera, float Interpolation)
{
	pCamera->UpdateViewportsAndScissorRects(pCommandList);
	pCommandList->SetGraphicsRootSignature(m_RootSignature);
	pCamera->UpdateShaderVariables(pCommandList);

	for (int i = 0; i < m_ObjectShaderCount; ++i)
		m_ObjectShaders[i].Render(pCommandList, pCamera, Interpolation);

	if (m_Player) m_Player->Render(pCommandList, pCamera);
}

void GameplayScene::Update(float DeltaTime)
{
	for (int i = 0; i < m_ObjectShaderCount; ++i)
		m_ObjectShaders[i].Update(DeltaTime);
	if (m_Player) m_Player->Update(DeltaTime);
}

void GameplayScene::ReleaseUploadBuffers()
{
	if (m_ObjectShaders)
	{
		for (int i = 0; i < m_ObjectShaderCount; ++i)
			m_ObjectShaders[i].ReleaseUploadBuffers();
	}
}
 