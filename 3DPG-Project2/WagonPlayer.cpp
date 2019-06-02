#include "stdafx.h"
#include "WagonPlayer.h"
#include "OBJMesh.h"
#include "GameFramework.h"

_3DGP_USE_
DX_USE

WagonPlayer::WagonPlayer(ID3D12Device * pDevice, ID3D12GraphicsCommandList * pCommandList, ID3D12RootSignature * pGraphicsRootSignature) 
{
	Mesh* pMesh = new OBJMesh(pDevice, pCommandList, "model.obj");
	SetMesh(pMesh);

	m_Camera = ChangeCamera(Camera::MODE::THIRD_PERSON, 0.f);
	CreateShaderVariables(pDevice, pCommandList);

	m_Position = XMFLOAT3(0.f, 0.f, -50.f);

	//Shader* pShader = new PlayerShader;
	//pShader->CreateShader(pDevice, pGraphicsRootSignature);
	//SetShader(pShader);
}

WagonPlayer::~WagonPlayer()
{
}

_3DGP_USE_
DX_USE

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

		SetFriction(125.0f);
		SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));

		SetMaxVelocityXZ(400.0f);
		SetMaxVelocityY(400.0f);

		OnCameraChange(CurrentCameraMode, NewCameraMode);
		m_Camera->SetTimeLag(0.0f);
		m_Camera->SetOffset(XMFLOAT3(0.0f, 1.0f, 0.0f));

		break;

	case Camera::MODE::THIRD_PERSON:
		SetFriction(250.f);
		SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));

		SetMaxVelocityXZ(125.0f);
		SetMaxVelocityY(400.0f);

		OnCameraChange(CurrentCameraMode, NewCameraMode);
		m_Camera->SetTimeLag(0.25f);
		m_Camera->SetOffset(XMFLOAT3(0.0f, 200.0f, -500.f));
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
