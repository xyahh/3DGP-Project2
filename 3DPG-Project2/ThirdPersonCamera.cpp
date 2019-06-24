#include "stdafx.h"
#include "ThirdPersonCamera.h"
#include "Player.h"

_3DGP_USE_
DX_USE

ThirdPersonCamera::ThirdPersonCamera(Camera * pCamera)
	: Camera(pCamera)
{
	m_CameraMode = Camera::MODE::THIRD_PERSON;
}

ThirdPersonCamera::~ThirdPersonCamera()
{
}

void ThirdPersonCamera::Update(const DX XMFLOAT3 & LookAt, float DeltaTime)
{
	if (!m_Player) return;
	
	XMVECTOR Offset = XMLoadFloat3(&m_Offset);

	//Extract only the Rotation Matrix from the Player
	XMMATRIX RotMat = m_Player->GetWorldTransformMatrix();
	RotMat.r[3] = XMVectorZero();
	XMVectorSetW(RotMat.r[3], 1.f);

	Offset = XMVector3Transform(Offset, RotMat);
	XMStoreFloat3(&m_Position, XMVectorAdd(XMLoadFloat3(&LookAt), Offset));
}

void ThirdPersonCamera::SetTarget(const DX XMFLOAT3 & LookAt)
{	
	XMFLOAT4X4 LookAtMat;
	XMStoreFloat4x4(&LookAtMat, DX XMMatrixLookAtLH(XMLoadFloat3(&m_Position), XMLoadFloat3(&LookAt), m_Player->GetUpVector()));

	m_Right = XMFLOAT3(LookAtMat._11, LookAtMat._21, LookAtMat._31);
	m_Up = XMFLOAT3(LookAtMat._12, LookAtMat._22, LookAtMat._32);
	m_Look = XMFLOAT3(LookAtMat._13, LookAtMat._23, LookAtMat._33);
}
