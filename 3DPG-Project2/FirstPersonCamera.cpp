#include "stdafx.h"
#include "FirstPersonCamera.h"
#include "Player.h"

_3DGP_USE_
DX_USE

FirstPersonCamera::FirstPersonCamera(Camera * pCamera)
	: Camera(pCamera)
{
	m_CameraMode = Camera::MODE::FIRST_PERSON;
}

FirstPersonCamera::~FirstPersonCamera()
{
}

void FirstPersonCamera::Rotate(float Pitch, float Yaw, float Roll)
{

	XMVECTOR Right = XMLoadFloat3(&m_Right);
	XMVECTOR Up = XMLoadFloat3(&m_Up);
	XMVECTOR Look = XMLoadFloat3(&m_Look);

	if (Pitch)
	{
		//Rotate around Cameras's Right Vector instead.
		XMMATRIX RotMat = XMMatrixRotationAxis(Right, XMConvertToRadians(Pitch));

		Right = XMVector3TransformNormal(Right, RotMat);
		Up = XMVector3TransformNormal(Up, RotMat);
		Look = XMVector3TransformNormal(Look, RotMat);
	}

	if (Yaw)
	{
		XMMATRIX RotMat = XMMatrixRotationAxis(XMLoadFloat3(&m_Player->GetUp()), XMConvertToRadians(Yaw));

		Right = XMVector3TransformNormal(Right, RotMat);
		Up = XMVector3TransformNormal(Up, RotMat);
		Look = XMVector3TransformNormal(Look, RotMat);
	}

	if (Roll)
	{
		XMVECTOR Position = XMLoadFloat3(&m_Position);
		XMVECTOR PlayerPos = XMLoadFloat3(&m_Player->GetPosition());

		XMMATRIX RotMat = XMMatrixRotationAxis(XMLoadFloat3(&m_Player->GetLook()), XMConvertToRadians(Roll));

		Right = XMVector3TransformNormal(Right, RotMat);
		Up = XMVector3TransformNormal(Up, RotMat);
		Look = XMVector3TransformNormal(Look, RotMat);

		Position = XMVectorSubtract(Position, PlayerPos);
		Position = XMVector3TransformCoord(Position, RotMat);
		Position = XMVectorAdd(Position, PlayerPos);

		XMStoreFloat3(&m_Position, Position);
	}

	XMStoreFloat3(&m_Right, Right);
	XMStoreFloat3(&m_Up, Up);
	XMStoreFloat3(&m_Look, Look);

}

void FirstPersonCamera::Update(const DX XMFLOAT3 & LookAt, float DeltaTime)
{
	XMVECTOR Offset = XMLoadFloat3(&m_Offset);

	//Extract only the Rotation Matrix from the Player
	XMMATRIX RotMat = m_Player->GetWorldTransformMatrix();
	RotMat.r[3] = XMVectorZero();
	XMVectorSetW(RotMat.r[3], 1.f);

	Offset = XMVector3Transform(Offset, RotMat);
	XMStoreFloat3(&m_Position, XMVectorAdd(XMLoadFloat3(&LookAt), Offset));

	XMFLOAT4X4 LookAtMat;
	XMStoreFloat4x4(&LookAtMat, DX XMMatrixLookToLH(XMLoadFloat3(&m_Position), m_Player->GetLookVector(), m_Player->GetUpVector()));

	m_Right = XMFLOAT3(LookAtMat._11, LookAtMat._21, LookAtMat._31);
	m_Up = XMFLOAT3(LookAtMat._12, LookAtMat._22, LookAtMat._32);
	m_Look = XMFLOAT3(LookAtMat._13, LookAtMat._23, LookAtMat._33);
}
