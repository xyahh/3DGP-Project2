#include "stdafx.h"
#include "HeightMapTerrain.h"
#include "HeightMapGridMesh.h"

_3DGP_USE_
DX_USE

HeightMapTerrain::HeightMapTerrain(ID3D12Device * pDevice, ID3D12GraphicsCommandList * pCommandList, LPCTSTR pFilename, 
	int terrain_width, int terrain_depth, int block_width, int block_depth, const DX XMFLOAT3 & scale, const DX XMFLOAT4 & color)
{
	m_TerrainWidth = terrain_width;
	m_TerrainDepth = terrain_depth;
	m_TerrainScale = scale;

	int cxQuadsPerBlock = block_width - 1;
	int czQuadsBerBlock = block_depth - 1;

	m_HeightMapImage = new HeightMapImage(pFilename, m_TerrainWidth, m_TerrainDepth, m_TerrainScale);

	int cxBlocks = (m_TerrainWidth - 1) / cxQuadsPerBlock;
	int czBlocks = (m_TerrainDepth - 1) / czQuadsBerBlock;

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
			int( z / m_TerrainScale.z)
		);

	return XMFLOAT3();
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

float HeightMapTerrain::GetTerrainWidth() const
{
	return m_TerrainWidth * m_TerrainScale.x;
}

float HeightMapTerrain::GetTerrainDepth() const
{
	return m_TerrainDepth * m_TerrainScale.z;
}
