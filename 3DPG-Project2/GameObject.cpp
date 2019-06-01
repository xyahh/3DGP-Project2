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

void GameObject::PreRender()
{
}

void GameObject::Render(ID3D12GraphicsCommandList * pCommandList, Camera* pCamera)
{
	PreRender();

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

XMFLOAT3 GameObject::GetPosition()
{
	return XMFLOAT3(m_World._41, m_World._42, m_World._43);
}

XMFLOAT3 GameObject::GetLook()
{
	XMFLOAT3 Look(m_World._31, m_World._32, m_World._33);
	XMStoreFloat3(&Look, XMVector3Normalize(XMLoadFloat3(&Look)));
	return Look;
}

XMFLOAT3 GameObject::GetUp()
{
	XMFLOAT3 Up(m_World._21, m_World._22, m_World._23);
	XMStoreFloat3(&Up, XMVector3Normalize(XMLoadFloat3(&Up)));
	return Up;
}

XMFLOAT3 GameObject::GetRight()
{
	XMFLOAT3 Right(m_World._11, m_World._12, m_World._13);
	XMStoreFloat3(&Right, XMVector3Normalize(XMLoadFloat3(&Right)));
	return Right;
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

void GameObject::MoveStrafe(float Distance)
{
	XMFLOAT3 Position = GetPosition();
	XMFLOAT3 Right = GetRight();
	XMStoreFloat3(&Position, XMVectorAdd(XMLoadFloat3(&Position), XMVectorScale(XMLoadFloat3(&Right), Distance)));
	SetPosition(Position);
}

void GameObject::MoveUp(float Distance)
{
	XMFLOAT3 Position = GetPosition();
	XMFLOAT3 Up = GetUp();
	XMStoreFloat3(&Position, XMVectorAdd(XMLoadFloat3(&Position), XMVectorScale(XMLoadFloat3(&Up), Distance)));
	SetPosition(Position);
}

void GameObject::MoveForward(float Distance)
{
	XMFLOAT3 Position = GetPosition();
	XMFLOAT3 Forward = GetLook();
	XMStoreFloat3(&Position, XMVectorAdd(XMLoadFloat3(&Position), XMVectorScale(XMLoadFloat3(&Forward), Distance)));
	SetPosition(Position);
}

void GameObject::Rotate(float Pitch, float Yaw, float Roll)
{
	XMMATRIX RotMax = XMMatrixRotationRollPitchYaw(
		XMConvertToRadians(Pitch), 
		XMConvertToRadians(Yaw), 
		XMConvertToRadians(Roll));
	XMStoreFloat4x4(&m_World, XMMatrixMultiply(RotMax, XMLoadFloat4x4(&m_World)));
}
