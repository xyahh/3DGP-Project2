#include "stdafx.h"
#include "ObjectsShader.h"

#include "GameFramework.h"
#include "WagonPlayer.h"

#include "RotatingObject.h"
#include "DiffusedCubeMesh.h"
#include "OBJMesh.h"

_3DGP_USE_
DX_USE

ObjectsShader::ObjectsShader()
{
}

ObjectsShader::~ObjectsShader()
{
}

D3D12_INPUT_LAYOUT_DESC ObjectsShader::CreateInputLayout()
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

D3D12_SHADER_BYTECODE ObjectsShader::CreateVertexShader(MWRL ComPtr<ID3DBlob>* pShaderBlob)
{
	D3D12_SHADER_BYTECODE ByteCode;
	*pShaderBlob = CompileShader(L"Shaders.hlsl", "VSDiffused", "vs_5_1");
	ByteCode.pShaderBytecode = (*pShaderBlob)->GetBufferPointer();
	ByteCode.BytecodeLength = (*pShaderBlob)->GetBufferSize();
	return ByteCode;
}

D3D12_SHADER_BYTECODE ObjectsShader::CreatePixelShader(MWRL ComPtr<ID3DBlob>* pShaderBlob)
{
	D3D12_SHADER_BYTECODE ByteCode;
	*pShaderBlob = CompileShader(L"Shaders.hlsl", "PSDiffused", "ps_5_1");
	ByteCode.pShaderBytecode = (*pShaderBlob)->GetBufferPointer();
	ByteCode.BytecodeLength = (*pShaderBlob)->GetBufferSize();
	return ByteCode;
}

void ObjectsShader::CreateShader(ID3D12Device * pDevice, ID3D12RootSignature * pRootSignature)
{
	m_PipelineStateCount = 1;
	m_PipelineStates = new MWRL ComPtr<ID3D12PipelineState>[m_PipelineStateCount];
	Shader::CreateShader(pDevice, pRootSignature);
}

void ObjectsShader::BuildObjects(ID3D12Device * pDevice, ID3D12GraphicsCommandList * pCommandList)
{
	WagonPlayer* pPlayer = dynamic_cast<WagonPlayer*>(GameFramework::Get()->GetCurrentPlayer());
	if (!pPlayer)
	{
		printf("Player is not wagon player!\n");
		return;
	}

	Mesh* pMesh = new OBJMesh(pDevice, pCommandList, "Rail.obj", XMFLOAT3(75.f, 75.f, -75.f), XMFLOAT3(0.f, 0.f, 0.f));
	CreateShaderVariables(pDevice, pCommandList);

}

void ObjectsShader::ReleaseUploadBuffers()
{
	for (auto& i : m_Objects)
	{
		if (i) i->ReleaseUploadBuffers();
	}
}

void ObjectsShader::Update(float DeltaTime)
{
	for (auto& i : m_Objects)
		i->Update(DeltaTime);
}

void ObjectsShader::ReleaseObjects()
{
	for (auto& i : m_Objects)
	{
		if (i) delete i;
	}
	m_Objects.clear();
}

void ObjectsShader::Render(ID3D12GraphicsCommandList * pCommandList, Camera * pCamera, float Interpolation)
{
	Shader::Render(pCommandList, pCamera);
	for (auto& i : m_Objects)
		i->Render(pCommandList, pCamera);
}
