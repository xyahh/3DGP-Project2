#include "stdafx.h"
#include "GameObject.h"

_3DGP_USE_
DX_USE

GameObject::GameObject()
{
	XMStoreFloat4x4(&m_World, XMMatrixIdentity());
}

GameObject::~GameObject()
{
	if (m_Mesh)		
		m_Mesh->Release();
	if (m_Shader)
	{
		m_Shader->ReleaseShaderVariables();
		m_Shader->Release();
	}
}

void GameObject::ReleaseUploadBuffers()
{
	if (m_Mesh) m_Mesh->ReleaseUploadBuffers();
}

void GameObject::SetMesh(Mesh * pMesh)
{
	if (m_Mesh) m_Mesh->Release();
	m_Mesh = pMesh;
	if (m_Mesh) m_Mesh->AddRef();
}

void GameObject::SetShader(Shader * pShader)
{
	if (m_Shader) m_Shader->Release();
	m_Shader = pShader;
	if (m_Shader) m_Shader->AddRef();
}

void GameObject::Update(float DeltaTime)
{
}

void GameObject::Render(ID3D12GraphicsCommandList * pCommandList, Camera* pCamera)
{
	UpdateShaderVariables(pCommandList);

	if (m_Shader) m_Shader->Render(pCommandList, pCamera);
	if (m_Mesh) m_Mesh->Render(pCommandList);
}

void GameObject::Rotate(const DX XMFLOAT3& Axis, float Angle)
{
	XMMATRIX RotMat = XMMatrixRotationAxis(XMLoadFloat3(&Axis), XMConvertToRadians(Angle));
	XMMATRIX World = RotMat * XMLoadFloat4x4(&m_World);
	XMStoreFloat4x4(&m_World, World);
}

void GameObject::CreateShaderVariables(ID3D12Device * pDevice, ID3D12GraphicsCommandList * pCommandList)
{
}

void GameObject::UpdateShaderVariables(ID3D12GraphicsCommandList * pCommandList)
{
	XMFLOAT4X4 World;
	XMStoreFloat4x4(&World, XMMatrixTranspose(XMLoadFloat4x4(&m_World)));
	pCommandList->SetGraphicsRoot32BitConstants(0, 16, &World, 0);
}

void GameObject::ReleaseShaderVariables()
{
}

XMFLOAT3 GameObject::GetPosition() const
{
	return XMFLOAT3(m_World._41, m_World._42, m_World._43);
}

XMFLOAT3 GameObject::GetRight() const
{
	XMFLOAT3 Right;
	XMStoreFloat3(&Right, GetRightVector());
	return Right;
}

XMFLOAT3 GameObject::GetUp() const
{
	XMFLOAT3 Up;
	XMStoreFloat3(&Up, GetUpVector());
	return Up;
}

XMFLOAT3 GameObject::GetLook() const
{
	XMFLOAT3 Look;
	XMStoreFloat3(&Look, GetLookVector());
	return Look;
}

DX XMVECTOR XM_CALLCONV GameObject::GetPositionVector() const
{
	return XMLoadFloat3(&GetPosition());
}

DX XMVECTOR XM_CALLCONV GameObject::GetRightVector() const
{
	XMFLOAT3 Right(m_World._11, m_World._12, m_World._13);
	return XMVector3Normalize(XMLoadFloat3(&Right));
}

DX XMVECTOR XM_CALLCONV GameObject::GetUpVector() const
{
	XMFLOAT3 Up(m_World._21, m_World._22, m_World._23);
	return XMVector3Normalize(XMLoadFloat3(&Up));
}

DX XMVECTOR XM_CALLCONV GameObject::GetLookVector() const
{
	XMFLOAT3 Look(m_World._31, m_World._32, m_World._33);
	return XMVector3Normalize(XMLoadFloat3(&Look));
}

void GameObject::SetPosition(const DX XMFLOAT3 & Position)
{
	m_World._41 = Position.x;
	m_World._42 = Position.y;
	m_World._43 = Position.z;
}

void GameObject::SetPosition(float x, float y, float z)
{
	m_World._41 = x;
	m_World._42 = y;
	m_World._43 = z;
}

void GameObject::MoveRight(float Distance)
{
	SetPosition(XMVectorAdd(GetPositionVector(), XMVectorScale(GetRightVector(), Distance)));
}

void GameObject::MoveUp(float Distance)
{
	SetPosition(XMVectorAdd(GetPositionVector(), XMVectorScale(GetUpVector(), Distance)));
}

void GameObject::MoveForward(float Distance)
{
	SetPosition(XMVectorAdd(GetPositionVector(), XMVectorScale(GetLookVector(), Distance)));
}

void GameObject::Rotate(float Pitch, float Yaw, float Roll)
{
	XMMATRIX RotMax = XMMatrixRotationRollPitchYaw(
		XMConvertToRadians(Pitch), 
		XMConvertToRadians(Yaw), 
		XMConvertToRadians(Roll));
	XMStoreFloat4x4(&m_World, XMMatrixMultiply(RotMax, XMLoadFloat4x4(&m_World)));
}

void XM_CALLCONV GameObject::SetPosition(DX XMVECTOR_P0 Position)
{
	XMFLOAT3 Pos;
	XMStoreFloat3(&Pos, Position);
	SetPosition(Pos);
}

void GameObject::SetRight(const DX XMFLOAT3 & Right)
{
	m_World._11 = Right.x;
	m_World._12 = Right.y;
	m_World._13 = Right.z;
}

void GameObject::SetUp(const DX XMFLOAT3 & Up)
{
	m_World._21 = Up.x;
	m_World._22 = Up.y;
	m_World._23 = Up.z;
}

void GameObject::SetLook(const DX XMFLOAT3 & Look)
{
	m_World._31 = Look.x;
	m_World._32 = Look.y;
	m_World._33 = Look.z;
}

void XM_CALLCONV GameObject::SetRight(DX XMVECTOR_P0 Right)
{
	XMFLOAT3 InRight;
	XMStoreFloat3(&InRight, Right);
	SetRight(InRight);
}

void XM_CALLCONV GameObject::SetUp(DX XMVECTOR_P0 Up)
{
	XMFLOAT3 InUp;
	XMStoreFloat3(&InUp, Up);
	SetUp(InUp);
}

void XM_CALLCONV GameObject::SetLook(DX XMVECTOR_P0 Look)
{
	XMFLOAT3 InLook;
	XMStoreFloat3(&InLook, Look);
	SetLook(InLook);
}

void GameObject::SetWorldTransform(const DX XMFLOAT4X4 & World)
{
	m_World = World;
}

DX XMFLOAT4X4 GameObject::GetWorldTransform() const
{
	return m_World;
}
