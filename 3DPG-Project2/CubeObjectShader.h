#pragma once
#include "InstancingShader.h"

_3DGP_BEGIN_

class CubeObjectShader : public InstancingShader
{
public:
	CubeObjectShader();
	virtual ~CubeObjectShader();

	virtual void	BuildObjects(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList);

};

_3DGP_END_

