#include "stdafx.h"
#include "OBJMesh.h"

#include "DiffusedVertex.h"
#include <sstream>

enum ATTRIBUTE_TYPE { VERTEX, TEXCOORD, NORMAL, POLYGON };

STD map<STD string, ATTRIBUTE_TYPE> gAttributeMap =
{
	{"v",	ATTRIBUTE_TYPE::VERTEX		},
	{"vt",	ATTRIBUTE_TYPE::TEXCOORD	},
	{"vn",	ATTRIBUTE_TYPE::NORMAL		},
	{"f",	ATTRIBUTE_TYPE::POLYGON		}
};

_3DGP_USE_
DX_USE

DX XMFLOAT4 InterpolateColor(const DX XMFLOAT4& A, const DX XMFLOAT4& B)
{
	DX XMFLOAT4 outColor;
	XMStoreFloat4(&outColor, XMVectorLerp(XMLoadFloat4(&A), XMLoadFloat4(&B), 0.75f));
	return outColor;
}



OBJMesh::OBJMesh(ID3D12Device * pDevice, ID3D12GraphicsCommandList * pCommandList, const STD string & filepath
	, const DX XMFLOAT4& DominantColor, const DX XMFLOAT3& Scale, const DX XMFLOAT3& Offset)
	: Mesh(pDevice, pCommandList)
{
	STD vector<DiffusedVertex>	pVertices;
	STD vector<UINT>			pIndices;

	DX XMVECTOR MaxValues = DX XMVectorZero(); //Zeros
	DX XMVECTOR MinValues = DX XMVectorTrueInt(); //Max value 0xFFFFFFFF

	m_PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	//Read OBJ File
	{
		STD fstream in(filepath, STD ios::in | STD ios::binary);
		assert(in && "OBJ File not found!");
		STD string attribute;
		float x, y, z;
		char slash;
		bool TexCoordsEnabled = false;
		bool NormalsEnabled = false;

		for (STD string line; STD getline(in, line);)
		{
			if (line.size() == 0) continue;

			STD stringstream lineparser(line);
			lineparser >> attribute;
			switch (gAttributeMap[attribute])
			{
			case ATTRIBUTE_TYPE::VERTEX:
				if (lineparser >> x >> y >> z)
				{
					DX XMFLOAT3 Pos = DX XMFLOAT3(
						(x * Scale.x) + Offset.x,
						(y * Scale.y) + Offset.y,
						(z * Scale.z) + Offset.z);
					pVertices.emplace_back(Pos, InterpolateColor(RANDOM_COLOR, DominantColor));

					MaxValues = XMVectorMax(MaxValues, XMLoadFloat3(&Pos));
					MinValues = XMVectorMin(MinValues, XMLoadFloat3(&Pos));
				}
					
				break;
			case ATTRIBUTE_TYPE::TEXCOORD:
				TexCoordsEnabled = true;
				break;
			case ATTRIBUTE_TYPE::NORMAL:
				NormalsEnabled = true;
				break;
			case ATTRIBUTE_TYPE::POLYGON:
			{
				UINT vertex_index, texture_index, normal_index;
				STD vector<UINT> pPolygon;
				while (lineparser >> vertex_index)
				{
					--vertex_index;
					pPolygon.emplace_back(vertex_index); //Indices in OBJ file start with 1 not 0
					
					lineparser >> slash;
					if (TexCoordsEnabled)
					{
						if (!(lineparser >> texture_index))
							break;
						--texture_index;

					}

					lineparser >> slash;
					if (NormalsEnabled)
					{
						if (!(lineparser >> normal_index))
							break;
						--normal_index;
					}
				}
				
				for(UINT& i : pPolygon)
				{
					pIndices.emplace_back(i);
				}
				break;
			}
			}
		}
		in.close();
	}
	
	DX XMFLOAT3 Extents;
	DX XMStoreFloat3(&Extents, XMVectorScale(XMVectorSubtract(MaxValues, MinValues), 0.5f));

	DX XMFLOAT3 Center;
	DX XMStoreFloat3(&Center, XMVectorScale(XMVectorAdd(MaxValues, MinValues), 0.5f));

	m_BoundingBox = DX BoundingOrientedBox(Center, Extents, DX XMFLOAT4(0.f, 0.f, 0.f, 1.f));

	//Create Vertex Buffer
	{
		m_VertexCount = static_cast<UINT>(pVertices.size());
		UINT Stride = sizeof(DiffusedVertex);
		m_VertexBuffer = CreateBufferResource(pDevice, pCommandList, pVertices.data(), m_VertexCount * Stride, 
			D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_VertexUploadBuffer);

		m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
		m_VertexBufferView.SizeInBytes = m_VertexCount * Stride;
		m_VertexBufferView.StrideInBytes = Stride;
	}

	//Create Index Buffer
	{
		m_IndexCount = static_cast<UINT>(pIndices.size());

		m_IndexBuffer = CreateBufferResource(pDevice, pCommandList, pIndices.data(), m_IndexCount * sizeof(UINT), 
			D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_IndexUploadBuffer);

		m_IndexBufferView.BufferLocation = m_IndexBuffer->GetGPUVirtualAddress();
		m_IndexBufferView.SizeInBytes = m_IndexCount * sizeof(UINT);
		m_IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	}	

}

OBJMesh::~OBJMesh()
{
}
