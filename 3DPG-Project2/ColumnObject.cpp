#include "stdafx.h"
#include "ColumnObject.h"

_3DGP_USE_
DX_USE

ColumnObject::ColumnObject()
{
	XMStoreFloat4x4(&m_ColWorld, XMMatrixIdentity());
}


ColumnObject::~ColumnObject()
{
}

DX XMMATRIX XM_CALLCONV ColumnObject::GetColumnWorldTransform()
{
	return XMLoadFloat4x4(&m_ColWorld);
}

void XM_CALLCONV ColumnObject::CalculateColumnTransform(const DX XMFLOAT3& Pos, DX XMVECTOR_P0 LookVector, HeightMapTerrain* pTerrain)
{
	DX XMFLOAT3 Look;
	XMStoreFloat3(&Look, LookVector);

	DX XMFLOAT3 Up;
	XMStoreFloat3(&Up, gWorldUp);

	DX XMFLOAT3 Right;
	XMStoreFloat3(&Right, XMVector3Normalize(XMVector3Cross(gWorldUp, LookVector)));

	float Height = pTerrain->GetHeight(Pos.x, Pos.z);
	float DeltaHeight = Pos.y - Height;


	m_ColWorld._11 = Right.x;	m_ColWorld._12 = Right.y;	m_ColWorld._13 = Right.z;
	m_ColWorld._21 = Up.x;		m_ColWorld._22 = Up.y;		m_ColWorld._23 = Up.z;	
	m_ColWorld._31 = Look.x;	m_ColWorld._32 = Look.y;	m_ColWorld._33 = Look.z;
	m_ColWorld._41 = Pos.x;		m_ColWorld._42 = Pos.y;		m_ColWorld._43 = Pos.z; 

	XMStoreFloat4x4(&m_ColWorld, XMMatrixMultiply(XMMatrixScaling(1.f, DeltaHeight, 1.f), XMLoadFloat4x4(&m_ColWorld)));
}
