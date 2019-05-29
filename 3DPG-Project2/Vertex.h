#pragma once

_3DGP_BEGIN_

class Vertex
{
public:
	Vertex();
	Vertex(const DX XMFLOAT3& Position);
	Vertex(float x, float y, float z);

	virtual ~Vertex();

protected:
	DX XMFLOAT3 m_Position;
};

_3DGP_END_