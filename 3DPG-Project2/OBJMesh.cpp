#include "stdafx.h"
#include "OBJMesh.h"

#include "DiffusedVertex.h"
#include <sstream>

enum ATTRIBUTE_TYPE { VERTEX, TEXCOORD, NORMAL, POLYGON };

std::map<std::string, ATTRIBUTE_TYPE> gAttributeMap =
{
	{"v",	ATTRIBUTE_TYPE::VERTEX		},
	{"vt",	ATTRIBUTE_TYPE::TEXCOORD	},
	{"vn",	ATTRIBUTE_TYPE::NORMAL		},
	{"f",	ATTRIBUTE_TYPE::POLYGON		}
};

_3DGP_USE_

OBJMesh::OBJMesh(ID3D12Device * pDevice, ID3D12GraphicsCommandList * pCommandList, const std::string & filepath) 
	: Mesh(pDevice, pCommandList)
{
	std::vector<DiffusedVertex> pVertices;
	std::vector<UINT>			pIndices;

	m_PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	//Read OBJ File
	{
		std::fstream in(filepath, std::ios::in | std::ios::binary);
		assert(in && "OBJ File not found!");
		std::string attribute;
		float x, y, z;
		char slash;
		bool TexCoordsEnabled = false;
		bool NormalsEnabled = false;
		for (std::string line; std::getline(in, line);)
		{
			if (line.size() == 0) continue;

			std::stringstream lineparser(line);
			lineparser >> attribute;
			switch (gAttributeMap[attribute])
			{
			case ATTRIBUTE_TYPE::VERTEX:
				if(lineparser >> x >> y >> z)
					pVertices.emplace_back(DX XMFLOAT3(x, y, z), RANDOM_COLOR);
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
				std::vector<UINT> pPolygon;
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
