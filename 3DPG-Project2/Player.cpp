#include "stdafx.h"
#include "Player.h"

#include "FirstPersonCamera.h"
#include "ThirdPersonCamera.h"
#include "SpaceshipCamera.h"

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

void Player::Move(ULONG Direction, float Distance, bool bVelocity)
{
	XMVECTOR Displacement = XMVectorZero();
	XMVECTOR Look = GetLookVector();
	XMVECTOR Right = GetRightVector();
	XMVECTOR Up = GetUpVector();

	if (Direction)
	{
		if (Direction & DIR_FORWARD) Displacement  = XMVectorAdd(Displacement, XMVectorScale(Look, Distance));
		if (Direction & DIR_BACKWARD) Displacement = XMVectorSubtract(Displacement, XMVectorScale(Look, Distance));

		if (Direction & DIR_RIGHT) Displacement = XMVectorAdd(Displacement, XMVectorScale(Right, Distance));
		if (Direction & DIR_LEFT) Displacement	= XMVectorSubtract(Displacement, XMVectorScale(Right, Distance));

		if (Direction & DIR_UP) Displacement	= XMVectorAdd(Displacement, XMVectorScale(Up, Distance));
		if (Direction & DIR_DOWN) Displacement	= XMVectorSubtract(Displacement, XMVectorScale(Up, Distance));
	
		XMFLOAT3 Result;
		XMStoreFloat3(&Result, Displacement);
		Move(Result, bVelocity);
	}	
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
	case Camera::MODE::THIRD_PERSON:
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

		m_Camera->Rotate(Pitch, Yaw, Roll);

		if (Yaw)
		{
			XMMATRIX RotMat = XMMatrixRotationAxis(Up, XMConvertToRadians(Yaw));
			
			Look = XMVector3TransformNormal(Look, RotMat);
			Right = XMVector3TransformNormal(Right, RotMat);
		}

		break;
	case Camera::MODE::SPACESHIP:
	{
		m_Camera->Rotate(Pitch, Yaw, Roll);
		if (Pitch)
		{
			XMMATRIX RotMat = XMMatrixRotationAxis(Right, XMConvertToRadians(Pitch));
			Look = XMVector3TransformNormal(Look, RotMat);
			Up   = XMVector3TransformNormal(Up, RotMat);
		}
		if (Yaw)
		{
			XMMATRIX RotMat = XMMatrixRotationAxis(Up, XMConvertToRadians(Yaw));
			Look = XMVector3TransformNormal(Look, RotMat);
			Right = XMVector3TransformNormal(Right, RotMat);
		}
		if (Roll)
		{
			XMMATRIX RotMat = XMMatrixRotationAxis(Look, XMConvertToRadians(Roll));
			Up = XMVector3TransformNormal(Up, RotMat);
			Right = XMVector3TransformNormal(Right, RotMat);
		}
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

	if (CameraMode == Camera::MODE::THIRD_PERSON) 
		m_Camera->Update(GetPosition(), DeltaTime);

	if (m_CameraUpdatedContext) OnCameraUpdateCallback(DeltaTime);

	if (CameraMode == Camera::MODE::THIRD_PERSON) 
		m_Camera->SetLookAt(GetPosition());

	m_Camera->RegenerateViewMatrix();

	Length = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_Velocity)));
	float Deceleration = m_Friction * DeltaTime;

	if (Deceleration > Length) Deceleration = Length;

	XMStoreFloat3(&m_Velocity, XMVectorAdd(XMLoadFloat3(&m_Velocity), XMVectorScale(XMVector3Normalize(XMLoadFloat3(&m_Velocity)), -Deceleration)));
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

void Player::OnCameraChange(Camera::MODE CurrentCameraMode, Camera::MODE NewCameraMode)
{
	Camera *pNewCamera = NULL;
	switch (NewCameraMode)
	{
	case Camera::MODE::FIRST_PERSON:
		pNewCamera = new FirstPersonCamera(m_Camera);
		break;
	case Camera::MODE::THIRD_PERSON:
		pNewCamera = new ThirdPersonCamera(m_Camera);
		break;
	case Camera::MODE::SPACESHIP:
		pNewCamera = new SpaceshipCamera(m_Camera);
		break;
	}

	if (CurrentCameraMode == Camera::MODE::SPACESHIP)
	{

		XMVECTOR RightXZ = GetRightVector();
		XMVectorSetY(RightXZ, 0.f);

		XMVECTOR LookXZ = GetLookVector(); 
		XMVectorSetY(LookXZ, 0.f);

		SetRight(XMVector3Normalize(RightXZ));
		SetLook(XMVector3Normalize(LookXZ));
		SetUp(XMFLOAT3(0.f, 1.f, 0.f));

		XMFLOAT3 DefLook(0.f, 0.f, 1.f);

		m_Rotation.x = 0.f;
		m_Rotation.y = XMVectorGetX(XMVector3AngleBetweenNormals(XMLoadFloat3(&DefLook), LookXZ));
		m_Rotation.z = 0.f;

		if (XMVectorGetX(LookXZ) < 0.f) m_Rotation.y *= -1.f;
	}
	else if (NewCameraMode == Camera::MODE::SPACESHIP && m_Camera)
	{
		SetRight(m_Camera->GetRight());
		SetUp(m_Camera->GetUp());
		SetLook(m_Camera->GetLook());
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
	if (CameraMode == Camera::MODE::THIRD_PERSON) 
		GameObject::Render(pCommandList, pCamera);
}
