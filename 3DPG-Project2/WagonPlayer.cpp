#include "stdafx.h"
#include "WagonPlayer.h"
#include "GameFramework.h"

#include "OBJMesh.h"
#include "DiffusedCubeMesh.h"

_3DGP_USE_
DX_USE

WagonPlayer::WagonPlayer(ID3D12Device * pDevice, ID3D12GraphicsCommandList * pCommandList, ID3D12RootSignature * pGraphicsRootSignature) 
{
	SetMesh		(new OBJMesh(pDevice, pCommandList, "Wagon1.obj", XMFLOAT3(75.f, 75.f, -75.f), XMFLOAT3(0.f, 0.f, 0.f)));
	SetSubMesh	(new OBJMesh(pDevice, pCommandList, "Wagon2.obj", XMFLOAT3(75.f, 75.f, -75.f), XMFLOAT3(0.f, 0.f, -100.f)));
	SetRailMesh	(new DiffusedCubeMesh(pDevice, pCommandList, 30.f, 30.f, 30.f));

	m_Camera = ChangeCamera(Camera::MODE::THIRD_PERSON, 0.f);
	CreateShaderVariables(pDevice, pCommandList);

	m_Position = XMFLOAT3(0.f, 0.f, -50.f);
}

WagonPlayer::~WagonPlayer()
{
}

void WagonPlayer::SetSubMesh(Mesh * pMesh)
{
	if (m_SubWagonMesh) m_SubWagonMesh->Release();
	m_SubWagonMesh = pMesh;
	if (m_SubWagonMesh) m_SubWagonMesh->AddRef();
}

void WagonPlayer::SetRailMesh(Mesh * pMesh)
{
	if (m_RailMesh) m_RailMesh->Release();
	m_RailMesh = pMesh;
	if (m_RailMesh) m_RailMesh->AddRef();
}

Camera * WagonPlayer::ChangeCamera(Camera::MODE NewCameraMode, float DeltaTime)
{
	Camera::MODE CurrentCameraMode = (m_Camera) ? m_Camera->GetMode() : Camera::MODE::NONE;
	
	if (CurrentCameraMode == NewCameraMode) return m_Camera;

	int WinWidth, WinHeight;
	GameFramework::Get()->GetWindowSize(&WinWidth, &WinHeight);
	float AspectRatio = GET_ASPECT_RATIO(WinWidth, WinHeight);

	switch (NewCameraMode)
	{
	case Camera::MODE::FIRST_PERSON:

		SetFriction(200.0f);
		SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));

		SetMaxVelocityXZ(125.0f);
		SetMaxVelocityY(400.0f);

		OnCameraChange(CurrentCameraMode, NewCameraMode);
		m_Camera->SetTimeLag(0.0f);
		m_Camera->SetOffset(XMFLOAT3(0.0f, 1.f, 0.0f));
		break;

	case Camera::MODE::SPACESHIP:

		SetFriction(50.f);
		SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));

		SetMaxVelocityXZ(400.0f);
		SetMaxVelocityY(400.0f);

		OnCameraChange(CurrentCameraMode, NewCameraMode);
		m_Camera->SetTimeLag(0.0f);
		m_Camera->SetOffset(XMFLOAT3(0.0f, 1.0f, 0.0f));

		break;

	case Camera::MODE::THIRD_PERSON:
		SetFriction(50.f);
		SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));

		SetMaxVelocityXZ(400.f);
		SetMaxVelocityY(400.0f);

		OnCameraChange(CurrentCameraMode, NewCameraMode);
		m_Camera->SetTimeLag(0.25f);
		m_Camera->SetOffset(XMFLOAT3(0.0f, 150.0f, -200.f));
		break;

	default:
		break;
	}

	m_Camera->SetPosition(XMFLOAT3(0.f, 0.f, m_Position.z + 0.1f)); //To avoid Eye == Target

	m_Camera->GenerateProjMatrix(1.01f, 5000.0f, AspectRatio, 60.0f);
	m_Camera->SetViewport(0, 0, WinWidth, WinHeight, 0.0f, 1.0f);
	m_Camera->SetScissorRect(0, 0, WinWidth, WinHeight);

	Update(DeltaTime);

	return m_Camera;
}

void WagonPlayer::Update(float DeltaTime)
{
	



	/* Increase Lifetime of every Rail */
	for (auto& pRails : m_Rails)
		pRails.Update(DeltaTime);

	/* Check for Rails that are past their Lifetime */
	for (auto i = m_Rails.begin(); i != m_Rails.end();)
	{
		if (i->IsExpired()) i = m_Rails.erase(i);
		else break; //Stop loop when first encounter an object with lifetime remaining (objects added later have more lifetime remaining)
	}
}

void WagonPlayer::Render(ID3D12GraphicsCommandList * pCommandList, Camera * pCamera)
{
	Player::Render(pCommandList, pCamera);
	if (m_SubWagonMesh) m_SubWagonMesh->Render(pCommandList);
	for (auto& pRails : m_Rails)
		pRails.Render(pCommandList, pCamera);
}

bool WagonPlayer::ProcessInput()
{
	static UCHAR pKeyBuffer[256];
	if (GetKeyboardState(pKeyBuffer))
	{
		if (KEY_PRESSED(pKeyBuffer, VK_F1))
		{
			ChangeCamera(Camera::MODE::FIRST_PERSON, Timer::GetDeltaTime());
			return true;
		}
		if (KEY_PRESSED(pKeyBuffer, VK_F2))
		{
			ChangeCamera(Camera::MODE::THIRD_PERSON, Timer::GetDeltaTime());
			return true;
		}

		if (KEY_PRESSED(pKeyBuffer, VK_F3))
		{
			ChangeCamera(Camera::MODE::SPACESHIP, Timer::GetDeltaTime());
			return true;
		}
	}
	return false;
}
