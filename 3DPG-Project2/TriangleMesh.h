#pragma once
#include "Mesh.h"

_3DGP_BEGIN_

class TriangleMesh : public Mesh
{
public:
	TriangleMesh(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList);
	~TriangleMesh();
};

_3DGP_END_