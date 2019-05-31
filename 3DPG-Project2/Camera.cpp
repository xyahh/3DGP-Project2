#include "stdafx.h"
#include "Camera.h"

_3DGP_USE_
DX_USE

Camera::Camera()
{
	XMStoreFloat4x4(&m_View, XMMatrixIdentity());
	XMStoreFloat4x4(&m_Proj, XMMatrixIdentity());

	m_Viewport = { 0.f, 0.f, 500.f, 500.f, 0.f, 1.f };
	m_ScissorRect = { 0, 0, 500, 500 };
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
	pCommandList->SetGraphicsRoot32BitConstants(1, 16, &ViewT, 0);

	XMFLOAT4X4 ProjT;
	XMStoreFloat4x4(&ProjT, XMMatrixTranspose(XMLoadFloat4x4(&m_Proj)));
	pCommandList->SetGraphicsRoot32BitConstants(1, 16, &ProjT, 16);
}

void Camera::GenerateViewMatrix(const DX XMFLOAT3 & Position, const DX XMFLOAT3 & LookAt, const DX XMFLOAT3 & Up)
{
	XMStoreFloat4x4(&m_View, XMMatrixLookAtLH(XMLoadFloat3(&Position), XMLoadFloat3(&LookAt), XMLoadFloat3(&Up)));
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
