#include "stdafx.h"
#include "DiffusedVertex.h"

_3DGP_USE_

DiffusedVertex::DiffusedVertex() 
	: Vertex(), m_Diffuse(0.f, 0.f, 0.f, 0.f)
{
}

DiffusedVertex::DiffusedVertex(float x, float y, float z, const DX XMFLOAT4 & Diffuse) 
	: Vertex(x, y, z), m_Diffuse(Diffuse)
{
}

DiffusedVertex::DiffusedVertex(const DX XMFLOAT3 & Position, const DX XMFLOAT4 & Diffuse)
	: Vertex(Position), m_Diffuse(Diffuse)
{
}

DiffusedVertex::~DiffusedVertex()
{
}
