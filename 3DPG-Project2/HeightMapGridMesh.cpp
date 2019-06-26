#include "stdafx.h"
#include "HeightMapGridMesh.h"
#include "HeightMapImage.h"
#include "DiffusedVertex.h"

_3DGP_USE_
DX_USE

HeightMapGridMesh::HeightMapGridMesh(ID3D12Device * pDevice, ID3D12GraphicsCommandList * pCommandList, 
	int xStart, int zStart, int width, int depth, const DX XMFLOAT3 & scale, const DX XMFLOAT4 & color, void * pContext)
	: Mesh(pDevice, pCommandList)
{
	m_Width = width;
	m_Depth = depth;

	m_VertexCount = m_Width * m_Depth;
	UINT Stride = sizeof(DiffusedVertex);

	m_Scale = scale;

	m_PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;


	float MaxHeight = 0.f;
	//Vertices
	{
		DiffusedVertex* pVertices = new DiffusedVertex[m_VertexCount];
		for (int i = 0, z = zStart; z < (zStart + m_Depth); z++)
		{
			for (int x = xStart; x < (xStart + m_Width); x++, i++)
			{

				XMFLOAT4 inColor = OnGetColor(x, z, pContext);
				float Height = OnGetHeight(x, z, pContext);
				MaxHeight = max(Height, MaxHeight);
				XMFLOAT3 inPos = XMFLOAT3((x*m_Scale.x), Height, (z * m_Scale.z));
				XMStoreFloat4(&inColor, XMVectorAdd(XMLoadFloat4(&inColor), XMLoadFloat4(&color)));
				pVertices[i] = DiffusedVertex(inPos, inColor);
			}
		}

		float fx = (float)(xStart + m_Width) * m_Scale.x / 2.f;
		float fz = (float)(zStart + m_Depth) * m_Scale.z / 2.f;
		float fy = MaxHeight / 2.f;

		m_BoundingBox = BoundingOrientedBox(
			XMFLOAT3((float)xStart*m_Scale.x + fx, fy, zStart*m_Scale.x + fz),
			XMFLOAT3(fx, fy, fz),
			XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));


		m_VertexBuffer = ::CreateBufferResource(pDevice, pCommandList, pVertices, Stride * m_VertexCount, D3D12_HEAP_TYPE_DEFAULT,
			D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_VertexUploadBuffer);

		m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
		m_VertexBufferView.StrideInBytes = Stride;
		m_VertexBufferView.SizeInBytes = Stride * m_VertexCount;

		delete[] pVertices;
	}


	//Indices
	{
		m_IndexCount = ((m_Width * 2)*(m_Depth - 1)) + ((m_Depth - 1) - 1);
		UINT* pIndices = new UINT[m_IndexCount];

		for (int j = 0, z = 0; z < m_Depth - 1; ++z)
		{
			if ((z % 2) == 0)
			{
				for (int x = 0; x < m_Width; ++x)
				{
					/* end of last row is left unfinished so join it with the start of this row */
					if ((x == 0) && (z > 0)) pIndices[j++] = (UINT)(x + (z * m_Width));

					/* down, up, down up... since primitive topology is Triangle Strip, this creates the zigzag effect (triangles) |\|\|\|  */
					pIndices[j++] = (UINT)(x + (z * m_Width)); //down
					pIndices[j++] = (UINT)((x + (z * m_Width)) + m_Width); //up
				}
			}
			else
			{
				for (int x = m_Width - 1; x >= 0; x--)
				{
					if (x == (m_Width - 1)) pIndices[j++] = (UINT)(x + (z * m_Width));

					pIndices[j++] = (UINT)(x + (z * m_Width));
					pIndices[j++] = (UINT)((x + (z * m_Width)) + m_Width);
				}
			}
		}
		m_IndexBuffer = ::CreateBufferResource(pDevice, pCommandList, pIndices, sizeof(UINT) * m_IndexCount, 
			D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_IndexUploadBuffer);

		m_IndexBufferView.BufferLocation = m_IndexBuffer->GetGPUVirtualAddress();
		m_IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
		m_IndexBufferView.SizeInBytes = sizeof(UINT) * m_IndexCount;

		delete[] pIndices;
	}
	
}

HeightMapGridMesh::~HeightMapGridMesh()
{
}

DX XMFLOAT3 HeightMapGridMesh::GetScale() const
{
	return m_Scale;
}

int HeightMapGridMesh::GetWidth() const
{
	return m_Width;
}

int HeightMapGridMesh::GetDepth() const
{
	return m_Depth;
}

float HeightMapGridMesh::OnGetHeight(int x, int z, void * pContext)
{
	HeightMapImage* pHeightMapImage = (HeightMapImage*)pContext;
	return static_cast<float>(pHeightMapImage->GetHeight((float)x, (float)z)) * m_Scale.y;
}

XMFLOAT4 HeightMapGridMesh::OnGetColor(int x_, int z_, void * pContext)
{
	XMFLOAT3 LightDirection_Pre = XMFLOAT3(-1.0f, 1.0f, 1.0f);
	XMVECTOR LightDir = XMVector3Normalize(XMLoadFloat3(&LightDirection_Pre));

	HeightMapImage* pHeightMapImage = (HeightMapImage*)pContext;

	XMFLOAT4 IncidentLightColor_Pre(0.9f, 0.8f, 0.4f, 1.f);
	XMVECTOR IncidentLightColor = XMLoadFloat4(&IncidentLightColor_Pre);

	float fScale = 0.f;
	fScale += XMVectorGetX(XMVector3Dot(XMLoadFloat3(&pHeightMapImage->GetHeightMapNormal(x_  , z_  , m_Scale)), LightDir));
	fScale += XMVectorGetX(XMVector3Dot(XMLoadFloat3(&pHeightMapImage->GetHeightMapNormal(x_+1, z_  , m_Scale)), LightDir));
	fScale += XMVectorGetX(XMVector3Dot(XMLoadFloat3(&pHeightMapImage->GetHeightMapNormal(x_+1, z_+1, m_Scale)), LightDir));
	fScale += XMVectorGetX(XMVector3Dot(XMLoadFloat3(&pHeightMapImage->GetHeightMapNormal(x_  , z_+1, m_Scale)), LightDir));

	fScale = (fScale / 4.0f) + 0.05f;

	if (fScale > 1.0f) 
		fScale = 1.0f;


	//simulate an ambient lighting i.e. do not let fScale become 0 and have the COlor be in complete darkness
	if (fScale < 0.25f) 
		fScale = 0.25f; 

	XMFLOAT4 Color;
	XMStoreFloat4(&Color, XMVectorScale(IncidentLightColor, fScale));
	if (OnGetHeight(x_, z_, pContext) == 0.0f)
		Color = XMFLOAT4(0.f, 0.1f, 0.9f, 0.f); //return water color!
	return Color;
}
