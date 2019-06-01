#pragma once
#include "Mesh.h"

_3DGP_BEGIN_

class DiffusedCubeMesh : public Mesh
{
public:
	DiffusedCubeMesh(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList,
		float Width = 2.f, float Height = 2.f, float Depth = 2.f);
	virtual ~DiffusedCubeMesh();
};

_3DGP_END_

