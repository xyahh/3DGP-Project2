#include "stdafx.h"
#include "GameObject.h"
#include "Shader.h"

_3DGP_USE_
DX_USE

GameObject::GameObject()
{
	XMStoreFloat4x4(&m_World, XMMatrixIdentity());
}

GameObject::~GameObject()
{
	if (m_Mesh)		m_Mesh->Release();
	if (m_Shader)	m_Shader->Release();
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

void GameObject::Render(ID3D12GraphicsCommandList * pCommandList)
{
	PreRender();
	if (m_Shader) m_Shader->Render(pCommandList);
	if (m_Mesh) m_Mesh->Render(pCommandList);
}
