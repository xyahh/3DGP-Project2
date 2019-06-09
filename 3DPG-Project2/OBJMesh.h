#pragma once
#include "Mesh.h"



_3DGP_BEGIN_

class OBJMesh : public Mesh
{
public:
	OBJMesh(ID3D12Device * pDevice, ID3D12GraphicsCommandList * pCommandList, const STD string & filepath
		, const DX XMFLOAT3& Scale = DX XMFLOAT3(1.f, 1.f, 1.f), const DX XMFLOAT3& Offset = DX XMFLOAT3(0.f, 0.f, 0.f));
	~OBJMesh();
};

_3DGP_END_
