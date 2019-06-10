#include "stdafx.h"
#include "ThirdPersonCamera.h"
#include "Player.h"

_3DGP_USE_
DX_USE

ThirdPersonCamera::ThirdPersonCamera(Camera * pCamera) 
	: Camera(pCamera)
{
	m_CameraMode = Camera::MODE::THIRD_PERSON;
	if (pCamera)
	{
		if (pCamera->GetMode() == Camera::MODE::SPACESHIP)
		{
			m_Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
			m_Look.y = 0.0f;
			m_Right.y = 0.0f;
			XMStoreFloat3(&m_Right, XMVector3Normalize(XMLoadFloat3(&m_Right)));
			XMStoreFloat3(&m_Look, XMVector3Normalize(XMLoadFloat3(&m_Look)));
		}
	}
}

ThirdPersonCamera::~ThirdPersonCamera()
{
}

void ThirdPersonCamera::Update(const DX XMFLOAT3 & LookAt, float DeltaTime)
{
	if (!m_Player) return;

	XMMATRIX PlayerRotMat;
	{
		XMFLOAT4X4 RotMat;

		XMFLOAT3 PlayerRight = m_Player->GetRight();
		XMFLOAT3 PlayerUp = m_Player->GetUp();
		XMFLOAT3 PlayerLook = m_Player->GetLook();

		RotMat._11 = PlayerRight.x;		RotMat._12 = PlayerRight.y;		RotMat._13 = PlayerRight.z;		RotMat._14 = 0.f;
		RotMat._21 = PlayerUp.x;		RotMat._22 = PlayerUp.y;		RotMat._23 = PlayerUp.z;		RotMat._24 = 0.f;
		RotMat._31 = PlayerLook.x;		RotMat._32 = PlayerLook.y;		RotMat._33 = PlayerLook.z;		RotMat._34 = 0.f;
		RotMat._41 = 0.f;				RotMat._42 = 0.f;				RotMat._43 = 0.f;				RotMat._44 = 1.f;

		PlayerRotMat = XMLoadFloat4x4(&RotMat);
	}

	XMVECTOR Offset = XMVector3TransformCoord(XMLoadFloat3(&m_Offset), PlayerRotMat);
	XMVECTOR Position = XMVectorAdd(m_Player->GetPositionVector(), Offset);

	XMVECTOR Direction = XMVectorSubtract(Position, XMLoadFloat3(&m_Position));
	float Length = XMVectorGetX(XMVector3Length(Direction));
	Direction = XMVector3Normalize(Direction);

	float TimeLagScale = (m_TimeLag) ? DeltaTime * (1.f / m_TimeLag) : 1.f;
	float Distance = Length * TimeLagScale;

	if (Distance > Length) Distance = Length;
	if(Length < 0.01f)		Distance = Length;

	if (Distance > 0.f)
	{
		XMStoreFloat3(&m_Position, XMVectorAdd(XMLoadFloat3(&m_Position), XMVectorScale(Direction, Distance)));
		SetLookAt(LookAt);
	}

}

void ThirdPersonCamera::SetLookAt(const DX XMFLOAT3 & LookAt)
{
	XMFLOAT4X4 LookAtMat;
	XMStoreFloat4x4(&LookAtMat, DX XMMatrixLookAtLH(XMLoadFloat3(&m_Position), XMLoadFloat3(&LookAt), XMLoadFloat3(&m_Player->GetUp())));

	m_Right = XMFLOAT3(LookAtMat._11, LookAtMat._21, LookAtMat._31);
	m_Up	= XMFLOAT3(LookAtMat._12, LookAtMat._22, LookAtMat._32);
	m_Look	= XMFLOAT3(LookAtMat._13, LookAtMat._23, LookAtMat._33);
}
