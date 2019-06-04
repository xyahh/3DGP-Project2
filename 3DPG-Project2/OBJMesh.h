#pragma once
#include "Mesh.h"



_3DGP_BEGIN_

class OBJMesh : public Mesh
{
public:
	OBJMesh(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList, const STD string& filepath);
	~OBJMesh();
};

_3DGP_END_
