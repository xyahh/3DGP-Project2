#include "stdafx.h"
#include "TriangleMesh.h"
#include "DiffusedVertex.h"

_3DGP_USE_
DX_USE

TriangleMesh::TriangleMesh(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList)
	: Mesh(pDevice, pCommandList)
{
	DiffusedVertex pVertices[] =
	{
		DiffusedVertex(XMFLOAT3(0.f, 0.5f, 0.f), XMFLOAT4(1.f, 0.f, 0.f, 1.f)),
		DiffusedVertex(XMFLOAT3(0.5f, -0.5f, 0.f), XMFLOAT4(0.f, 1.f, 0.f, 1.f)),
		DiffusedVertex(XMFLOAT3(-0.5f, -0.5f, 0.f), XMFLOAT4(0.f, 0.f, 1.f, 1.f))
	};

	m_Stride = sizeof(DiffusedVertex);
	m_Vertices = _countof(pVertices);
	m_PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_VertexBuffer = CreateBufferResource(pDevice, pCommandList, pVertices, m_Vertices * m_Stride, 
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_VertexUploadBuffer);

	m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
	m_VertexBufferView.SizeInBytes = m_Stride * m_Vertices;
	m_VertexBufferView.StrideInBytes = m_Stride;
}


TriangleMesh::~TriangleMesh()
{
}
