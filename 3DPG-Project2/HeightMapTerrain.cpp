#include "stdafx.h"
#include "HeightMapTerrain.h"
#include "HeightMapGridMesh.h"

_3DGP_USE_
DX_USE


HeightMapTerrain::HeightMapTerrain(ID3D12Device * pDevice, ID3D12GraphicsCommandList * pCommandList, LPCTSTR pFilename, int block_count_x, int block_count_z, const DX XMFLOAT3 & scale, const DX XMFLOAT4 & color)
{
	m_HeightMapImage = new HeightMapImage(pFilename);

	m_TerrainWidth = m_HeightMapImage->GetHeightMapWidth();
	m_TerrainDepth = m_HeightMapImage->GetHeightMapDepth();
	m_TerrainScale = scale;

	int block_width = m_TerrainWidth / block_count_x;
	int block_depth = m_TerrainDepth / block_count_z;

	int cxQuadsPerBlock = block_width - 1;
	int czQuadsBerBlock = block_depth - 1;

	//Center the terrain
	m_Offset.x = -GetTerrainWidth() * 0.5f;
	m_Offset.y = 0.f;
	m_Offset.z = -GetTerrainDepth() * 0.5f;

	SetPosition(m_Offset);


	int cxBlocks = block_count_x;
	int czBlocks = block_count_z;

	m_Meshes.reserve(cxBlocks * czBlocks);

	for (int z = 0, zStart = 0; z < czBlocks; ++z)
	{
		for (int x = 0, xStart = 0; x < cxBlocks; ++x)
		{
			xStart = x * cxQuadsPerBlock;
			zStart = z * czQuadsBerBlock;
			AddMesh(new HeightMapGridMesh(pDevice, pCommandList, xStart, zStart,
				block_width, block_depth, m_TerrainScale, color, m_HeightMapImage));
		}
	}
}

HeightMapTerrain::~HeightMapTerrain()
{
}

float HeightMapTerrain::GetHeight(float x, float z) const
{
	if (m_HeightMapImage)
		return m_HeightMapImage->GetHeight
		(
			x / m_TerrainScale.x, 
			z / m_TerrainScale.z
		) 
		* m_TerrainScale.y;
	return 0.f;
}

DX XMFLOAT3 HeightMapTerrain::GetNormal(float x, float z) const
{
	if (m_HeightMapImage)
		return m_HeightMapImage->GetHeightMapNormal
		(
			int( x / m_TerrainScale.x),
			int( z / m_TerrainScale.z),
			m_TerrainScale
		);

	return XMFLOAT3();
}

DX XMFLOAT3 HeightMapTerrain::GetOffset() const
{
	return m_Offset;
}

int HeightMapTerrain::GetHeightMapWidth() const
{
	if (m_HeightMapImage)
		return m_HeightMapImage->GetHeightMapWidth();
	return 0;
}

int HeightMapTerrain::GetHeightMapDepth() const
{
	if (m_HeightMapImage)
		return m_HeightMapImage->GetHeightMapDepth();
	return 0;
}

DX XMFLOAT3 HeightMapTerrain::GetScale() const
{
	return m_TerrainScale;
}

float HeightMapTerrain::GetTerrainWidth() const
{
	return m_TerrainWidth * m_TerrainScale.x;
}

float HeightMapTerrain::GetTerrainDepth() const
{
	return m_TerrainDepth * m_TerrainScale.z;
}
