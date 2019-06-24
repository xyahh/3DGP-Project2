#include "stdafx.h"
#include "DiffusedCubeMesh.h"
#include "DiffusedVertex.h"

_3DGP_USE_
DX_USE

DiffusedCubeMesh::DiffusedCubeMesh(ID3D12Device * pDevice, ID3D12GraphicsCommandList * pCommandList, float Width, float Height, float Depth)
	: Mesh(pDevice, pCommandList)
{

	//Vertex Buffer
	{
		float fx = Width * 0.5f;
		float fy = Height * 0.5f;
		float fz = Depth * 0.5f;

		m_BoundingBox = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fx, fy, fz), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

		DiffusedVertex pVertices[] =
		{
			DiffusedVertex(XMFLOAT3(-fx, +fy, -fz), RANDOM_COLOR),
			DiffusedVertex(XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR),
			DiffusedVertex(XMFLOAT3(+fx, +fy, +fz), RANDOM_COLOR),
			DiffusedVertex(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR),
			DiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR),
			DiffusedVertex(XMFLOAT3(+fx, -fy, -fz), RANDOM_COLOR),
			DiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR),
			DiffusedVertex(XMFLOAT3(-fx, -fy, +fz), RANDOM_COLOR)
		};
		m_VertexCount = _countof(pVertices);

		m_VertexBuffer = CreateBufferResource(pDevice, pCommandList, pVertices, sizeof(pVertices),
			D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_VertexUploadBuffer);

		m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
		m_VertexBufferView.StrideInBytes = sizeof(DiffusedVertex);
		m_VertexBufferView.SizeInBytes = sizeof(pVertices);
	}
	
	//Index Buffer
	{
		UINT pIndices[] =
		{
			// Front Face Triangles
			3, 1, 0,
			2, 1, 3,
			// Top Face Triangles
			0, 5, 4,
			1, 5, 0,
			// Back Face Triangles
			3, 4, 7,
			0, 4, 3,
			// Bottom Face Triangles
			1, 6, 5,
			2, 6, 1,
			// Left Face Triangles
			2, 7, 6,
			3, 7, 2,
			// Right Face Triangles
			6, 4, 5,
			7, 4, 6
		};
		m_IndexCount = _countof(pIndices);

		m_IndexBuffer = CreateBufferResource(pDevice, pCommandList, pIndices, sizeof(pIndices)
			, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_IndexUploadBuffer);

		m_IndexBufferView.BufferLocation = m_IndexBuffer->GetGPUVirtualAddress();
		m_IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
		m_IndexBufferView.SizeInBytes = sizeof(pIndices);
		
	}

}

DiffusedCubeMesh::~DiffusedCubeMesh()
{
}
