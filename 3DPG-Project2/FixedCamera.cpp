#include "stdafx.h"
#include "FixedCamera.h"

_3DGP_USE_
DX_USE

FixedCamera::FixedCamera(Camera * pCamera)
	: Camera(pCamera)
{
}

FixedCamera::~FixedCamera()
{
}

void FixedCamera::SetTarget(const DX XMFLOAT3 & LookAt)
{
	XMFLOAT4X4 LookAtMat;
	XMStoreFloat4x4(&LookAtMat, DX XMMatrixLookAtLH(XMLoadFloat3(&GetPosition()), XMLoadFloat3(&LookAt), XMLoadFloat3(&GetUp())));

	m_Right = XMFLOAT3(LookAtMat._11, LookAtMat._21, LookAtMat._31);
	m_Up = XMFLOAT3(LookAtMat._12, LookAtMat._22, LookAtMat._32);
	m_Look = XMFLOAT3(LookAtMat._13, LookAtMat._23, LookAtMat._33);
}
