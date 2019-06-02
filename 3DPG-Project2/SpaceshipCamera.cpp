#include "stdafx.h"
#include "SpaceshipCamera.h"
#include "Player.h"

_3DGP_USE_
DX_USE

SpaceshipCamera::SpaceshipCamera(Camera * pCamera) : Camera(pCamera)
{
	m_CameraMode = Camera::MODE::SPACESHIP;
}

SpaceshipCamera::~SpaceshipCamera()
{
}

void SpaceshipCamera::Rotate(float Pitch, float Yaw, float Roll)
{
	if (!m_Player) return;

	XMVECTOR Position		= XMLoadFloat3(&m_Position);

	XMVECTOR Right			= XMLoadFloat3(&m_Right);
	XMVECTOR Up				= XMLoadFloat3(&m_Up);
	XMVECTOR Look			= XMLoadFloat3(&m_Look);

	XMVECTOR PlayerPos		= XMLoadFloat3(&m_Player->GetPosition());

	/**/

	if (Pitch)
	{
		XMMATRIX RotMat = XMMatrixRotationAxis(XMLoadFloat3(&m_Player->GetRight()), XMConvertToRadians(Pitch));

		Right	= XMVector3TransformNormal(Right, RotMat);
		Up		= XMVector3TransformNormal(Up, RotMat);
		Look	= XMVector3TransformNormal(Look, RotMat);
		
		Position = XMVectorSubtract(Position, PlayerPos);
		Position = XMVector3TransformCoord(Position, RotMat);
		Position = XMVectorAdd(Position, PlayerPos);
	}
	if (Yaw)
	{
		XMMATRIX RotMat = XMMatrixRotationAxis(XMLoadFloat3(&m_Player->GetUp()), XMConvertToRadians(Yaw));

		Right	= XMVector3TransformNormal(Right, RotMat);
		Up		= XMVector3TransformNormal(Up, RotMat);
		Look	= XMVector3TransformNormal(Look, RotMat);

		Position = XMVectorSubtract(Position, PlayerPos);
		Position = XMVector3TransformCoord(Position, RotMat);
		Position = XMVectorAdd(Position, PlayerPos);
	}
	if (Roll)
	{
		XMMATRIX RotMat = XMMatrixRotationAxis(XMLoadFloat3(&m_Player->GetLook()), XMConvertToRadians(Roll));

		Right	= XMVector3TransformNormal(Right, RotMat);
		Up		= XMVector3TransformNormal(Up, RotMat);
		Look	= XMVector3TransformNormal(Look, RotMat);

		Position = XMVectorSubtract(Position, PlayerPos);
		Position = XMVector3TransformCoord(Position, RotMat);
		Position = XMVectorAdd(Position, PlayerPos);
	}

	XMStoreFloat3(&m_Position, Position);
	XMStoreFloat3(&m_Right, Right);
	XMStoreFloat3(&m_Up, Up);
	XMStoreFloat3(&m_Look, Look);
}
