#include "stdafx.h"
#include "DiffusedShader.h"

_3DGP_USE_

DiffusedShader::DiffusedShader()
{
}

DiffusedShader::~DiffusedShader()
{
}

D3D12_INPUT_LAYOUT_DESC DiffusedShader::CreateInputLayout()
{
	UINT InputElementDescCount = 2;
	D3D12_INPUT_ELEMENT_DESC* pInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[InputElementDescCount];

	pInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT	, 0, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pInputElementDescs[1] = { "COLOR"	, 0, DXGI_FORMAT_R32G32B32A32_FLOAT	, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC InputLayoutDesc;
	InputLayoutDesc.pInputElementDescs = pInputElementDescs;
	InputLayoutDesc.NumElements = InputElementDescCount;

	return InputLayoutDesc;
}

D3D12_SHADER_BYTECODE DiffusedShader::CreateVertexShader(MWRL ComPtr<ID3DBlob>* pShaderBlob)
{
	return CompileShaderFromFile(L"DiffusedShader.hlsl", "VSDiffused", "vs_5_1", pShaderBlob);
}

D3D12_SHADER_BYTECODE DiffusedShader::CreatePixelShader(MWRL ComPtr<ID3DBlob>* pShaderBlob)
{
	return CompileShaderFromFile(L"DiffusedShader.hlsl", "PSDiffused", "ps_5_1", pShaderBlob);
}

void DiffusedShader::CreateShader(ID3D12Device * pDevice, ID3D12RootSignature * pRootSignature)
{
	m_PipelineStateCount = 1;
	m_PipelineStates = new MWRL ComPtr<ID3D12PipelineState>[m_PipelineStateCount];

	Shader::CreateShader(pDevice, pRootSignature);
}
