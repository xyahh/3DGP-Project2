#include "stdafx.h"
#include "DiffusedCubeMesh.h"
#include "DiffusedVertex.h"

_3DGP_USE_
DX_USE

DiffusedCubeMesh::DiffusedCubeMesh(ID3D12Device * pDevice, ID3D12GraphicsCommandList * pCommandList, float Width, float Height, float Depth)
	: Mesh(pDevice, pCommandList)
{
	m_Vertices = 36;
	m_Stride = sizeof(DiffusedVertex);
	m_Offset = 0;
	m_Slot = 0;
	m_PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = Width  * 0.5f;
	float fy = Height * 0.5f;
	float fz = Depth  * 0.5f;

	DiffusedVertex pVertices[36];
	int i = 0;

	//ⓐ 앞면(Front) 사각형의 위쪽 삼각형
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, +fy, -fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, -fy, -fz), RANDOM_COLOR);
	//ⓑ 앞면(Front) 사각형의 아래쪽 삼각형
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, +fy, -fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, -fy, -fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR);
	//ⓒ 윗면(Top) 사각형의 위쪽 삼각형
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, +fy, +fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR);
	//ⓓ 윗면(Top) 사각형의 아래쪽 삼각형
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, +fy, -fz), RANDOM_COLOR);
	//ⓔ 뒷면(Back) 사각형의 위쪽 삼각형
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, -fy, +fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, +fy, +fz), RANDOM_COLOR);
	//ⓕ 뒷면(Back) 사각형의 아래쪽 삼각형
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, -fy, +fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, +fy, +fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR);
	//ⓖ 아래면(Bottom 사각형의 위쪽 삼각형
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, -fy, -fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR);
	//ⓗ 아래면(Bottom 사각형의 아래쪽 삼각형
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, -fy, +fz), RANDOM_COLOR);
	//ⓘ 옆면(Left) 사각형의 위쪽 삼각형
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, +fy, -fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR);
	//ⓙ 옆면(Left) 사각형의 아래쪽 삼각형
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, -fy, +fz), RANDOM_COLOR);
	//ⓚ 옆면(Right) 사각형의 위쪽 삼각형
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, +fy, +fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR);
	//ⓛ 옆면(Right) 사각형의 아래쪽 삼각형
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, -fy, -fz), RANDOM_COLOR);

	m_VertexBuffer = CreateBufferResource(pDevice, pCommandList, pVertices, m_Stride * m_Vertices, 
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_VertexBuffer);

	m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
	m_VertexBufferView.StrideInBytes = m_Stride;
	m_VertexBufferView.SizeInBytes = m_Stride * m_Vertices;

}

DiffusedCubeMesh::~DiffusedCubeMesh()
{
}
