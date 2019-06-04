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
