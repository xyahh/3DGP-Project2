#include "stdafx.h"
#include "FirstPersonCamera.h"
#include "Player.h"

_3DGP_USE_
DX_USE

FirstPersonCamera::FirstPersonCamera(Camera * pCamera)
	: Camera(pCamera)
{
	m_CameraMode = Camera::MODE::FIRST_PERSON;
	if (pCamera)
	{ 
		if (pCamera->GetMode() == Camera::MODE::SPACESHIP)
		{
			m_Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
			m_Right.y = 0.0f;
			m_Look.y = 0.0f;
			XMStoreFloat3(&m_Right, XMVector3Normalize(XMLoadFloat3(&m_Right)));
			XMStoreFloat3(&m_Look,  XMVector3Normalize(XMLoadFloat3(&m_Look)));
		}
	}
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
