#include "stdafx.h"
#include "TriangleMesh.h"
#include "DiffusedVertex.h"

_3DGP_USE_
DX_USE

TriangleMesh::TriangleMesh(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList)
	: Mesh(pDevice, pCommandList)
{
	m_Vertices = 3;
	m_Stride = sizeof(DiffusedVertex);
	m_PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	DiffusedVertex Vertices[3];
	Vertices[0] = DiffusedVertex(XMFLOAT3(0.f, 0.5f, 0.f), XMFLOAT4(1.f, 0.f, 0.f, 1.f));
	Vertices[1] = DiffusedVertex(XMFLOAT3(0.5f, -0.5f, 0.f), XMFLOAT4(0.f, 1.f, 0.f, 1.f));
	Vertices[2] = DiffusedVertex(XMFLOAT3(-0.5f, -0.5f, 0.f), XMFLOAT4(0.f, 0.f, 1.f, 1.f));

	m_VertexBuffer = CreateBufferResource(pDevice, pCommandList, Vertices, m_Vertices * m_Stride, 
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_VertexUploadBuffer.GetAddressOf());

	m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
	m_VertexBufferView.SizeInBytes = m_Stride * m_Vertices;
	m_VertexBufferView.StrideInBytes = m_Stride;
}


TriangleMesh::~TriangleMesh()
{
}
