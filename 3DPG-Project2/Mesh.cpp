#include "stdafx.h"
#include "Mesh.h"

_3DGP_USE_
DX_USE

Mesh::Mesh(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList)
{
}

Mesh::~Mesh()
{
	if (m_VertexBuffer) m_VertexBuffer->Release();
	if (m_VertexUploadBuffer) m_VertexUploadBuffer->Release();
}

void Mesh::ReleaseUploadBuffers()
{
	if (m_VertexUploadBuffer) m_VertexUploadBuffer->Release();
	m_VertexUploadBuffer = NULL;
}

void Mesh::Render(ID3D12GraphicsCommandList * pCommandList)
{
	pCommandList->IASetPrimitiveTopology(m_PrimitiveTopology);
	pCommandList->IASetVertexBuffers(m_Slot, 1, &m_VertexBufferView);
	pCommandList->DrawInstanced(m_Vertices, 1, m_Offset, 0);
}
