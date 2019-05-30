#pragma once
#include "Vertex.h"

_3DGP_BEGIN_

class DiffusedVertex : public Vertex
{
public:
	DiffusedVertex();
	DiffusedVertex(const DX XMFLOAT3& Position, const DX XMFLOAT4& Diffuse);
	~DiffusedVertex();

protected:

	DX XMFLOAT4 m_Diffuse;

};

_3DGP_END_

