#pragma once
#include "Shader.h"

_3DGP_BEGIN_

class DiffusedShader : public Shader
{
public:
	DiffusedShader();
	virtual ~DiffusedShader();

	virtual D3D12_INPUT_LAYOUT_DESC		CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE		CreateVertexShader(MWRL ComPtr<ID3DBlob>* pShaderBlob);
	virtual D3D12_SHADER_BYTECODE		CreatePixelShader(MWRL ComPtr<ID3DBlob>* pShaderBlob);
	virtual void						CreateShader(ID3D12Device* pDevice, ID3D12RootSignature* pRootSignature);

};

_3DGP_END_

