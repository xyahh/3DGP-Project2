#include "stdafx.h"
#include "DiffusedVertex.h"

_3DGP_USE_
DX_USE

DiffusedVertex::DiffusedVertex() 
	: Vertex(), m_Diffuse(0.f, 0.f, 0.f, 0.f)
{
}

DiffusedVertex::DiffusedVertex(float x, float y, float z, const XMFLOAT4 & Diffuse) 
	: Vertex(x, y, z), m_Diffuse(Diffuse)
{
}

DiffusedVertex::DiffusedVertex(const XMFLOAT3 & Position, const XMFLOAT4 & Diffuse)
	: Vertex(Position), m_Diffuse(Diffuse)
{
}

DiffusedVertex::~DiffusedVertex()
{
}
