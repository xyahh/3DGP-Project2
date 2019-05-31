#pragma once
#include "Shader.h"

_3DGP_BEGIN_

class DiffusedShader : public Shader
{
public:
	DiffusedShader();
	~DiffusedShader();

	virtual D3D12_INPUT_LAYOUT_DESC		CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE		CreateVertexShader(ID3DBlob** pShaderBlob);
	virtual D3D12_SHADER_BYTECODE		CreatePixelShader(ID3DBlob** pShaderBlob);
	virtual void						CreateShader(ID3D12Device* pDevice, ID3D12RootSignature* pRootSignature);

};

_3DGP_END_

