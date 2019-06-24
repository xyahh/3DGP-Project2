#pragma once
#include "HeightMapTerrain.h"
#include "InstancingShader.h"

_3DGP_BEGIN_

class TreeObjectsShader : public InstancingShader
{
public:
	TreeObjectsShader(HeightMapTerrain** pTerrain);
	~TreeObjectsShader();

	virtual void	BuildObjects(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList);
	
private:
	HeightMapTerrain** m_Terrain{ NULL };

};

_3DGP_END_