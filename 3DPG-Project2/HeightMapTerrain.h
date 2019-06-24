#pragma once
#include "GameObject.h"
#include "HeightMapImage.h"

_3DGP_BEGIN_

class HeightMapTerrain : public GameObject
{
public:
	HeightMapTerrain(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList, LPCTSTR pFilename, 
		int block_count_x, int block_count_z,  const DX XMFLOAT3& scale, const DX XMFLOAT4& color);
	virtual ~HeightMapTerrain();


	float GetHeight(float x, float z) const;
	DX XMFLOAT3 GetNormal(float x, float z) const;
	
	DX XMFLOAT3 GetOffset() const;

	int GetHeightMapWidth() const;
	int GetHeightMapDepth() const;

	DX XMFLOAT3 GetScale() const;

	float GetTerrainWidth() const;
	float GetTerrainDepth() const;



private:

	HeightMapImage* m_HeightMapImage{ NULL };
	
	DX XMFLOAT3		m_Offset;

	int				m_TerrainWidth;
	int				m_TerrainDepth;

	DX XMFLOAT3		m_TerrainScale;


};

_3DGP_END_
