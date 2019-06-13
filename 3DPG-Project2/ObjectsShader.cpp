#include "stdafx.h"
#include "ObjectsShader.h"

#include "GameFramework.h"
#include "WagonPlayer.h"

#include "RotatingObject.h"
#include "DiffusedCubeMesh.h"
#include "OBJMesh.h"

_3DGP_USE_
DX_USE

ObjectShader::ObjectShader()
{
}

ObjectShader::~ObjectShader()
{
}

D3D12_INPUT_LAYOUT_DESC ObjectShader::CreateInputLayout()
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

D3D12_SHADER_BYTECODE ObjectShader::CreateVertexShader(MWRL ComPtr<ID3DBlob>* pShaderBlob)
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

D3D12_SHADER_BYTECODE ObjectShader::CreatePixelShader(MWRL ComPtr<ID3DBlob>* pShaderBlob)
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

void ObjectShader::CreateShader(ID3D12Device * pDevice, ID3D12RootSignature * pRootSignature)
{
	m_PipelineStateCount = 1;
	m_PipelineStates = new MWRL ComPtr<ID3D12PipelineState>[m_PipelineStateCount];
	Shader::CreateShader(pDevice, pRootSignature);
}

void ObjectShader::BuildObjects(ID3D12Device * pDevice, ID3D12GraphicsCommandList * pCommandList)
{

}

void ObjectShader::ReleaseUploadBuffers()
{
	for (auto& i : m_Objects)
	{
		if (i) i->ReleaseUploadBuffers();
	}
}

void ObjectShader::Update(float DeltaTime)
{
	for (auto& i : m_Objects)
		i->Update(DeltaTime);
}

void ObjectShader::ReleaseObjects()
{
	for (auto& i : m_Objects)
	{
		if (i) delete i;
	}
	m_Objects.clear();
}

void ObjectShader::Render(ID3D12GraphicsCommandList * pCommandList, Camera * pCamera, float Interpolation)
{
	Shader::Render(pCommandList, pCamera);
	for (auto& i : m_Objects)
		i->Render(pCommandList, pCamera);
}
