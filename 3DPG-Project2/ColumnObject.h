#pragma once
#include "TimedObject.h"
#include "HeightMapTerrain.h"

_3DGP_BEGIN_

class ColumnObject : public TimedObject
{
public:
	ColumnObject();
	virtual ~ColumnObject();

	DX XMMATRIX XM_CALLCONV GetColumnWorldTransform();
	void XM_CALLCONV CalculateColumnTransform(const DX XMFLOAT3& Position, DX XMVECTOR_P0 LookVector, HeightMapTerrain* pTerrain);


private:

	DX XMFLOAT4X4 m_ColWorld;

};

_3DGP_END_