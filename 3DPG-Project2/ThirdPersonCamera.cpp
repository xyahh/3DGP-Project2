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

void ThirdPersonCamera::Rotate(float Pitch, float Yaw, float Roll)
{
	 XMVECTOR q = XMLoadFloat4(&m_Rotation);
	 XMVECTOR r = XMQuaternionRotationRollPitchYaw(
		 XMConvertToRadians(Pitch),
		 XMConvertToRadians(Yaw), 
		 XMConvertToRadians(Roll));
	 q = XMQuaternionMultiply(r, q);
	 XMStoreFloat4(&m_Rotation, q);
}

void ThirdPersonCamera::Update(const DX XMFLOAT3 & LookAt, float DeltaTime)
{
	XMMATRIX RotMat = XMMatrixRotationQuaternion(XMLoadFloat4(&m_Rotation));

	XMVECTOR Offset = XMLoadFloat3(&m_Offset);
	XMVECTOR Target = XMLoadFloat3(&LookAt);

	XMVECTOR Position = XMVectorAdd(Target, XMVector3Transform(Offset, RotMat));
	XMStoreFloat3(&m_Position, Position);
}

void ThirdPersonCamera::SetTarget(const DX XMFLOAT3 & Target)
{
	XMVECTOR Up = XMLoadFloat3(&gWorldUp);
	Up = XMVector3Transform(Up, XMMatrixRotationQuaternion(XMLoadFloat4(&m_Rotation)));

	XMFLOAT4X4 LookAtMat;
	XMStoreFloat4x4(&LookAtMat, DX XMMatrixLookAtLH(XMLoadFloat3(&m_Position), XMLoadFloat3(&Target), Up));

	m_Right = XMFLOAT3(LookAtMat._11, LookAtMat._21, LookAtMat._31);
	m_Up	= XMFLOAT3(LookAtMat._12, LookAtMat._22, LookAtMat._32);
	m_Look	= XMFLOAT3(LookAtMat._13, LookAtMat._23, LookAtMat._33);
}
