#pragma once
#include "Mesh.h"

_3DGP_BEGIN_

class HeightMapGridMesh : public Mesh
{
public:
	HeightMapGridMesh(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList, int xStart, int zStart, int width, int depth, 
		const DX XMFLOAT3& scale = XMFLOAT3(1.f, 1.f, 1.f), const DX XMFLOAT4& color = XMFLOAT4(1.f, 1.f, 0.f, 0.f), void *pContext = NULL);
	virtual ~HeightMapGridMesh();

	DX XMFLOAT3 GetScale() const;
	int GetWidth() const;
	int GetDepth() const;

	virtual float OnGetHeight(int x, int z, void* pContext);
	virtual XMFLOAT4 OnGetColor(int x, int z, void* pContext);


private:
	int			m_Width;
	int			m_Depth;
	DX XMFLOAT3 m_Scale;
};

_3DGP_END_