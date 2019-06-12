#include "stdafx.h"
#include "Mesh.h"

_3DGP_USE_
DX_USE

Mesh::Mesh(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList)
{
}

Mesh::~Mesh()
{
}

void Mesh::ReleaseUploadBuffers()
{
	m_VertexUploadBuffer.Reset();
	m_IndexUploadBuffer.Reset();
}

void Mesh::Render(ID3D12GraphicsCommandList * pCommandList)
{
	pCommandList->IASetVertexBuffers(m_Slot, 1, &m_VertexBufferView);
	Render(pCommandList, 1);
}

void Mesh::Render(ID3D12GraphicsCommandList * pCommandList, UINT InstanceCount)
{
	pCommandList->IASetPrimitiveTopology(m_PrimitiveTopology);
	if (m_IndexBuffer)
	{
		pCommandList->IASetIndexBuffer(&m_IndexBufferView);
		pCommandList->DrawIndexedInstanced(m_IndexCount, InstanceCount, m_StartIndex, m_BaseVertex, 0);
	}
	else
	{
		pCommandList->DrawInstanced(m_VertexCount, InstanceCount, m_Offset, 0);
	}
}

void Mesh::Render(ID3D12GraphicsCommandList * pCommandList, UINT InstanceCount, D3D12_VERTEX_BUFFER_VIEW InstancingBufferView)
{
	D3D12_VERTEX_BUFFER_VIEW pVertexBufferViews[] = { m_VertexBufferView, InstancingBufferView };
	pCommandList->IASetVertexBuffers(m_Slot, _countof(pVertexBufferViews), pVertexBufferViews);
	Render(pCommandList, InstanceCount);
}
