#include "stdafx.h"
#include "Camera.h"
#include "Player.h"

_3DGP_USE_
DX_USE

void Camera::Init()
{
	XMStoreFloat4x4(&m_View, XMMatrixIdentity());
	XMStoreFloat4x4(&m_Proj, XMMatrixIdentity());

	int CX = GetSystemMetrics(SM_CXSCREEN);
	int CY = GetSystemMetrics(SM_CYSCREEN);

	m_Viewport = { 0.f, 0.f, static_cast<float>(CX), static_cast<float>(CY), 0.f, 1.f };
	m_ScissorRect = { 0, 0, CX, CY };

	m_Position = XMFLOAT3(0.f, 0.f, 0.f);
	m_Right = XMFLOAT3(1.f, 0.f, 0.f);
	m_Up = gWorldUp;
	m_Look = XMFLOAT3(0.f, 0.f, 1.f);
	m_Rotation = XMFLOAT4(0.f, 0.f, 0.f, 1.f);

	m_Offset = XMFLOAT3(0.f, 0.f, 0.f);
	m_TimeLag = 0.f;
	m_Target = XMFLOAT3(0.f, 0.f, 0.f);
	m_CameraMode = Camera::MODE::NONE;
	m_Player = NULL;
}

Camera::Camera()
{
	Init();
}

Camera::Camera(Camera * pCamera)
{
	if (pCamera)
		*this = *pCamera;
	else
		Init();
}

Camera::~Camera()
{
}

void Camera::CreateShaderVariables(ID3D12Device * pDevice, ID3D12GraphicsCommandList * pCommandList)
{
}

void Camera::ReleaseShaderVariables()
{
}

void Camera::UpdateShaderVariables(ID3D12GraphicsCommandList * pCommandList)
{
	XMFLOAT4X4 ViewT;
	XMStoreFloat4x4(&ViewT, XMMatrixTranspose(XMLoadFloat4x4(&m_View)));
	pCommandList->SetGraphicsRoot32BitConstants(ROOT_INDEX::CAM, Count32Bits(ViewT), &ViewT, 0);

	XMFLOAT4X4 ProjT;
	XMStoreFloat4x4(&ProjT, XMMatrixTranspose(XMLoadFloat4x4(&m_Proj)));
	pCommandList->SetGraphicsRoot32BitConstants(ROOT_INDEX::CAM, Count32Bits(ProjT), &ProjT, Count32Bits(ViewT));
}

void Camera::GenerateViewMatrix()
{
	XMStoreFloat4x4(&m_View, XMMatrixLookAtLH(XMLoadFloat3(&m_Position), XMLoadFloat3(&m_Target), XMLoadFloat3(&m_Up)));
}

void Camera::GenerateViewMatrix(const DX XMFLOAT3 & Position, const DX XMFLOAT3 & LookAt, const DX XMFLOAT3 & Up)
{
	m_Position = Position;
	m_Target = LookAt;
	m_Up = Up;

	GenerateViewMatrix();
}

void Camera::RegenerateViewMatrix()
{
	XMVECTOR Position = XMLoadFloat3(&m_Position);

	XMVECTOR Look = XMVector3Normalize(XMLoadFloat3(&m_Look));
	XMStoreFloat3(&m_Look, Look);

	XMVECTOR Right = XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&m_Up), Look));
	XMStoreFloat3(&m_Right, Right);

	XMVECTOR Up = XMVector3Normalize(XMVector3Cross(Look, Right));
	XMStoreFloat3(&m_Up, Up);
	
	// Recalculate Inverse Matrix of Camera's World Transform (i.e. View Matrix)
	m_View._11	= m_Right.x;	 m_View._12 = m_Up.x;		m_View._13 = m_Look.x;
	m_View._21	= m_Right.y; 	 m_View._22 = m_Up.y;		m_View._23 = m_Look.y;
	m_View._31	= m_Right.z;	 m_View._32 = m_Up.z;		m_View._33 = m_Look.z;

	m_View._41 = -XMVectorGetX((XMVector3Dot(Position, Right)));
	m_View._42 = -XMVectorGetX((XMVector3Dot(Position, Up)));
	m_View._43 = -XMVectorGetX((XMVector3Dot(Position, Look)));
}

void Camera::GenerateProjMatrix(float Near, float Far, float AspectRatio, float FOVAngle)
{
	XMStoreFloat4x4(&m_Proj,XMMatrixPerspectiveFovLH(XMConvertToRadians(FOVAngle), AspectRatio, Near, Far));
}

void Camera::SetViewport(int xTopLeft, int yTopLeft, int Width, int Height, float MinDepth, float MaxDepth)
{
	m_Viewport.TopLeftX = static_cast<float>(xTopLeft);
	m_Viewport.TopLeftY = static_cast<float>(yTopLeft);
	m_Viewport.Width = static_cast<float>(Width);
	m_Viewport.Height = static_cast<float>(Height);

	m_Viewport.MinDepth = MinDepth;
	m_Viewport.MaxDepth = MaxDepth;
}

void Camera::SetScissorRect(LONG Left, LONG Top, LONG Right, LONG Bottom)
{
	m_ScissorRect.left = Left;
	m_ScissorRect.top = Top;
	m_ScissorRect.right = Right;
	m_ScissorRect.bottom = Bottom;
}

void Camera::UpdateViewportsAndScissorRects(ID3D12GraphicsCommandList * pCommandList)
{
	pCommandList->RSSetViewports(1, &m_Viewport);
	pCommandList->RSSetScissorRects(1, &m_ScissorRect);
}
