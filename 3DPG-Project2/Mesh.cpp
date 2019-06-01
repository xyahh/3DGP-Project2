#include "stdafx.h"
#include "Mesh.h"

_3DGP_USE_
DX_USE

Mesh::Mesh(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList)
{
}

Mesh::~Mesh()
{
	if (m_VertexBuffer) 
		m_VertexBuffer->Release();
	if (m_VertexUploadBuffer) 
		m_VertexUploadBuffer->Release();

	if (m_IndexBuffer)
		m_IndexBuffer->Release();
	if (m_IndexUploadBuffer)
		m_IndexUploadBuffer->Release();

}

void Mesh::ReleaseUploadBuffers()
{
	if (m_VertexUploadBuffer) 
		m_VertexUploadBuffer->Release();

	if(m_IndexUploadBuffer)
		m_IndexUploadBuffer->Release();

	m_VertexUploadBuffer = NULL;
	m_IndexUploadBuffer = NULL;
}

void Mesh::Render(ID3D12GraphicsCommandList * pCommandList)
{
	pCommandList->IASetPrimitiveTopology(m_PrimitiveTopology);
	pCommandList->IASetVertexBuffers(m_Slot, 1, &m_VertexBufferView);

	if (m_IndexBuffer)
	{
		pCommandList->IASetIndexBuffer(&m_IndexBufferView);
		pCommandList->DrawIndexedInstanced(m_IndexCount, 1, m_StartIndex, m_BaseVertex, 0);
	}
	else
	{
		pCommandList->DrawInstanced(m_VertexCount, 1, m_Offset, 0);
	}
}
