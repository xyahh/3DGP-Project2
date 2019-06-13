#include "stdafx.h"
#include "CubeObjectShader.h"


#include "RotatingObject.h"
#include "DiffusedCubeMesh.h"

_3DGP_USE_
DX_USE

CubeObjectShader::CubeObjectShader()
{
}


CubeObjectShader::~CubeObjectShader()
{
}

void CubeObjectShader::BuildObjects(ID3D12Device * pDevice, ID3D12GraphicsCommandList * pCommandList)
{
	Mesh* pMesh = new DiffusedCubeMesh(pDevice, pCommandList, 12.f, 12.f, 12.f);

	int xObjects = 10, yObjects = 10, zObjects = 10, i = 0;
	m_ObjectCount = (xObjects * 2 + 1) * (yObjects * 2 + 1) * (zObjects * 2 + 1);
	m_Objects.reserve(m_ObjectCount);
	
	float fxPitch = 12.0f * 2.5f;
	float fyPitch = 12.0f * 2.5f;
	float fzPitch = 12.0f * 2.5f;
	
	RotatingObject *pRotatingObject = NULL;
	for (int x = -xObjects; x <= xObjects; x++)
	{
		for (int y = -yObjects; y <= yObjects; y++)
		{
			for (int z = -zObjects; z <= zObjects; z++)
			{
				pRotatingObject = new RotatingObject;
				pRotatingObject->SetPosition(fxPitch*x, fyPitch*y, fzPitch*z);
				pRotatingObject->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
				pRotatingObject->SetAngularSpeed(50.0f*(++i % 10));
				m_Objects.emplace_back(pRotatingObject);
			}
		}
	}
	m_Objects[0]->SetMesh(pMesh);
	CreateShaderVariables(pDevice, pCommandList);
}
