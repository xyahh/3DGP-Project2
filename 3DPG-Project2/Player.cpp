#include "stdafx.h"
#include "Player.h"

#include "FirstPersonCamera.h"
#include "OrbitalCamera.h"

_3DGP_USE_
DX_USE

Player::Player()
	: m_Camera(NULL)
	, m_Velocity(0.f, 0.f, 0.f)
	, m_Gravity(0.f, 0.f, 0.f)
	, m_MaxVelocityXZ(0.f)
	, m_MaxVelocityY(0.f)
	, m_Friction(0.f)
	, m_Rotation(0.f, 0.f, 0.f)
	, m_PlayerUpdatedContext(NULL)
	, m_CameraUpdatedContext(NULL)
{
}

Player::~Player()
{
	ReleaseShaderVariables();
	if (m_Camera)
		delete m_Camera;
}

void Player::SetFriction(float Friction)
{
	m_Friction = Friction;
}

void Player::SetGravity(const DX XMFLOAT3 & Gravity)
{
	m_Gravity = Gravity;
}

void Player::SetMaxVelocityXZ(float MaxVelocity)
{
	m_MaxVelocityXZ = MaxVelocity;
}

void Player::SetMaxVelocityY(float MaxVelocity)
{
	m_MaxVelocityY = MaxVelocity;
}

void Player::SetVelocity(const DX XMFLOAT3 & Velocity)
{
	m_Velocity = Velocity;
}

DX XMFLOAT3 Player::GetVelocity() const
{
	return m_Velocity;
}

void Player::GetRotationValues(float * pPitchOut, float * pYawOut, float * pRollOut) const
{
	if (pPitchOut)	*pPitchOut = m_Rotation.x;
	if (pYawOut)	*pYawOut = m_Rotation.y;
	if (pRollOut)	*pRollOut = m_Rotation.z;
}

Camera * Player::GetCamera() const
{
	return m_Camera;
}

void Player::SetCamera(Camera * pCamera)
{
	m_Camera = pCamera;
}

void Player::Move(const DX XMFLOAT3 & Shift, bool bVelocity)
{
	if (bVelocity)
		XMStoreFloat3(&m_Velocity, XMVectorAdd(XMLoadFloat3(&m_Velocity), XMLoadFloat3(&Shift)));
	else
	{
		SetPosition(XMVectorAdd(GetPositionVector(), XMLoadFloat3(&Shift)));
		m_Camera->Move(Shift);
	}

}

void Player::Rotate(float Pitch, float Yaw, float Roll)
{

	XMVECTOR Right	= GetRightVector();
	XMVECTOR Up		= GetUpVector();
	XMVECTOR Look	= GetLookVector();

	Camera::MODE CameraMode = m_Camera->GetMode();
	switch (CameraMode)
	{
	case Camera::MODE::FIRST_PERSON:
	case Camera::MODE::ORBITAL:
		if (Pitch)
		{
			m_Rotation.x += Pitch;
			if (m_Rotation.x > +89.f) { Pitch -= (m_Rotation.x - 89.f); m_Rotation.x = +89.f; }
			if (m_Rotation.x < -89.f) { Pitch -= (m_Rotation.x + 89.f); m_Rotation.x = -89.f; }
		}
		if (Yaw)
		{
			m_Rotation.y += Yaw;
			if (m_Rotation.y > 360.f) m_Rotation.y -= 360.f;
			if (m_Rotation.y < 0.f)   m_Rotation.y += 360.f;
		}
		if (Roll)
		{
			m_Rotation.z += Roll;
			if (m_Rotation.z > +20.f) { Roll -= (m_Rotation.z - 20.f); m_Rotation.z = +20.f; }
			if (m_Rotation.z < -20.f) { Roll -= (m_Rotation.z + 20.f); m_Rotation.z = -20.f; }
		}

		//m_Camera->Rotate(Pitch, Yaw, Roll);

		if (Yaw)
		{
			XMMATRIX RotMat = XMMatrixRotationAxis(Up, XMConvertToRadians(Yaw));
			
			Look = XMVector3TransformNormal(Look, RotMat);
			Right = XMVector3TransformNormal(Right, RotMat);
		}
		break;
	}

	SetLook(XMVector3Normalize(Look));
	SetRight(XMVector3Normalize(XMVector3Cross(Up, Look)));
	SetUp(XMVector3Normalize(XMVector3Cross(Look, Right)));
}

void Player::Update(float DeltaTime)
{
	XMStoreFloat3(&m_Velocity, XMVectorAdd(XMLoadFloat3(&m_Velocity), XMVectorScale(XMLoadFloat3(&m_Gravity), DeltaTime)));

	float Length;
	
	Length = sqrtf(m_Velocity.x * m_Velocity.x + m_Velocity.z * m_Velocity.z);
	float MaxVelocityXZ = m_MaxVelocityXZ * DeltaTime;
	if (Length > m_MaxVelocityXZ)
	{
		m_Velocity.x *= (MaxVelocityXZ / Length);
		m_Velocity.z *= (MaxVelocityXZ / Length);
	}

	Length = abs(m_Velocity.y);
	float MaxVelocityY = m_MaxVelocityY * DeltaTime;
	if (Length > m_MaxVelocityY)
	{
		m_Velocity.y *= (MaxVelocityY / Length);
	}

	Move(m_Velocity, false);
	if (m_PlayerUpdatedContext) OnPlayerUpdateCallback(DeltaTime);

	Camera::MODE CameraMode = m_Camera->GetMode();

	m_Camera->Update(GetPosition(), DeltaTime);

	if (m_CameraUpdatedContext) OnCameraUpdateCallback(DeltaTime);

	if (CameraMode == Camera::MODE::ORBITAL) 
		m_Camera->SetTarget(GetPosition());

	m_Camera->RegenerateViewMatrix();

	Length = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_Velocity)));
	float Deceleration = m_Friction * DeltaTime;

	if (Deceleration > Length) Deceleration = Length;

	XMStoreFloat3(&m_Velocity, XMVectorAdd(XMLoadFloat3(&m_Velocity), XMVectorScale(XMVector3Normalize(XMLoadFloat3(&m_Velocity)), -Deceleration)));
}

void Player::OnPlayerUpdateCallback(float DeltaTime) 
{
}

void Player::SetPlayerUpdatedContext(LPVOID pContext) 
{ 
	m_PlayerUpdatedContext = pContext; 
}

void Player::OnCameraUpdateCallback(float DeltaTime) 
{
}

void Player::SetCameraUpdatedContext(LPVOID pContext) 
{ 
	m_CameraUpdatedContext = pContext; 
}

void Player::CreateShaderVariables(ID3D12Device * pDevice, ID3D12GraphicsCommandList * pCommandList)
{
	GameObject::CreateShaderVariables(pDevice, pCommandList);
	if (m_Camera) m_Camera->CreateShaderVariables(pDevice, pCommandList);
}

void Player::ReleaseShaderVariables()
{
	GameObject::ReleaseShaderVariables();
	if (m_Camera) m_Camera->ReleaseShaderVariables();
}

void Player::UpdateShaderVariables(ID3D12GraphicsCommandList * pCommandList)
{
	GameObject::UpdateShaderVariables(pCommandList);
}

Camera * Player::ChangeCamera(Camera::MODE NewCameraMode, float DeltaTime)
{
	return NULL;
}

void Player::OnCameraChange(Camera::MODE CurrentCameraMode, Camera::MODE NewCameraMode)
{
	Camera *pNewCamera = NULL;
	switch (NewCameraMode)
	{
	case Camera::MODE::FIRST_PERSON:
		pNewCamera = new FirstPersonCamera(m_Camera);
		break;
	case Camera::MODE::ORBITAL:
		pNewCamera = new OrbitalCamera(m_Camera);
		break;
	}
	if (pNewCamera)
	{
		pNewCamera->SetMode(NewCameraMode);
		pNewCamera->SetPlayer(this);
	}
	if (m_Camera) delete m_Camera;
	m_Camera = pNewCamera;
}

void Player::Render(ID3D12GraphicsCommandList * pCommandList, Camera * pCamera)
{
	Camera::MODE CameraMode = (pCamera) ? pCamera->GetMode() : Camera::MODE::NONE;
	if (CameraMode == Camera::MODE::ORBITAL || CameraMode == Camera::MODE::THIRD_PERSON)
		GameObject::Render(pCommandList, pCamera);
}



