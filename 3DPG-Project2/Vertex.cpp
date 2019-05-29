#include "stdafx.h"
#include "Vertex.h"

_3DGP_USE_
DX_USE

Vertex::Vertex() 
	: m_Position(0.f, 0.f, 0.f) 
{
}

Vertex::Vertex(const XMFLOAT3 & Position) 
	: m_Position(Position)
{ 
}

Vertex::Vertex(float x, float y, float z)
	: m_Position(x, y, z)
{
}

Vertex::~Vertex()
{
}
