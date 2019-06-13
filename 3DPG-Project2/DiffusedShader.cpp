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
	ID3DInclude* pInclude = new ShaderInclude;
	LPCVOID pData; UINT	Bytes;
	D3D12_SHADER_BYTECODE ByteCode;

	pInclude->Open(D3D_INCLUDE_LOCAL, "CBStruct.hlsl", NULL, &pData, &Bytes);
	ByteCode = CompileShaderFromFile(L"DiffusedShader.hlsl", "VSDiffused", "vs_5_1", pInclude, pShaderBlob);
	pInclude->Close(pData);

	delete pInclude;

	return ByteCode;
}

D3D12_SHADER_BYTECODE DiffusedShader::CreatePixelShader(MWRL ComPtr<ID3DBlob>* pShaderBlob)
{
	ID3DInclude* pInclude = new ShaderInclude;
	LPCVOID pData; UINT	Bytes;
	D3D12_SHADER_BYTECODE ByteCode;

	pInclude->Open(D3D_INCLUDE_LOCAL, "CBStruct.hlsl", NULL, &pData, &Bytes);
	ByteCode = CompileShaderFromFile(L"DiffusedShader.hlsl", "PSDiffused", "ps_5_1", pInclude, pShaderBlob);
	pInclude->Close(pData);

	delete pInclude;

	return ByteCode;
}

void DiffusedShader::CreateShader(ID3D12Device * pDevice, ID3D12RootSignature * pRootSignature)
{
	m_PipelineStateCount = 1;
	m_PipelineStates = new MWRL ComPtr<ID3D12PipelineState>[m_PipelineStateCount];

	Shader::CreateShader(pDevice, pRootSignature);
}
