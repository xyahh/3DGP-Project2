#pragma once
#include "Mesh.h"

_3DGP_BEGIN_

class TriangleMesh : public Mesh
{
public:
	TriangleMesh(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandLists, float Width = 1.f, float Height = 1.f);
	~TriangleMesh();
};

_3DGP_END_