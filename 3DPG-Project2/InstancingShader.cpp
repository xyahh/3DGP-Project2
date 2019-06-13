#include "stdafx.h"
#include "InstancingShader.h"

#include "RotatingObject.h"
#include "DiffusedCubeMesh.h"

_3DGP_USE_
DX_USE

InstancingShader::InstancingShader()
{
}


InstancingShader::~InstancingShader()
{
}

void InstancingShader::BuildObjects(ID3D12Device * pDevice, ID3D12GraphicsCommandList * pCommandList)
{
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

	DiffusedCubeMesh *pCubeMesh = new DiffusedCubeMesh(pDevice, pCommandList, 12.0f, 12.0f, 12.0f);
	m_Objects[0]->SetMesh(pCubeMesh);

	CreateShaderVariables(pDevice, pCommandList);
}

void InstancingShader::ReleaseObjects()
{
}

D3D12_INPUT_LAYOUT_DESC InstancingShader::CreateInputLayout()
{

	D3D12_INPUT_LAYOUT_DESC Desc;

	UINT InputDescCount = 7;
	D3D12_INPUT_ELEMENT_DESC* pInputDescs = new D3D12_INPUT_ELEMENT_DESC[InputDescCount];

	/* Per Vertex Input Elements */
	pInputDescs[0] =	{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT		, 0, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pInputDescs[1] =	{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT	, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	/* Per Instance Input Elements */
	pInputDescs[2] =	{ "WORLDMATRIX",	0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0,  D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1};
	pInputDescs[3] =	{ "WORLDMATRIX",	1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1};
	pInputDescs[4] =	{ "WORLDMATRIX",	2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1};
	pInputDescs[5] =	{ "WORLDMATRIX",	3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1};
	pInputDescs[6] =	{ "INSTANCECOLOR",	0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 64, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1};

	Desc.NumElements = InputDescCount;
	Desc.pInputElementDescs = pInputDescs;

	return Desc;
}

D3D12_SHADER_BYTECODE InstancingShader::CreateVertexShader(MWRL ComPtr<ID3DBlob>* pShaderBlob)
{
	ID3DInclude* pInclude = new ShaderInclude;
	LPCVOID pData; UINT	Bytes;
	D3D12_SHADER_BYTECODE ByteCode;

	pInclude->Open(D3D_INCLUDE_LOCAL, "CBStruct.hlsl", NULL, &pData, &Bytes);
	ByteCode = CompileShaderFromFile(L"InstancingShader.hlsl", "VSInstancing", "vs_5_1", pInclude, pShaderBlob);
	pInclude->Close(pData);

	delete pInclude;

	return ByteCode;
}

D3D12_SHADER_BYTECODE InstancingShader::CreatePixelShader(MWRL ComPtr<ID3DBlob>* pShaderBlob)
{
	ID3DInclude* pInclude = new ShaderInclude;
	LPCVOID pData; UINT	Bytes;
	D3D12_SHADER_BYTECODE ByteCode;

	pInclude->Open(D3D_INCLUDE_LOCAL, "CBStruct.hlsl", NULL, &pData, &Bytes);
	ByteCode = CompileShaderFromFile(L"InstancingShader.hlsl", "PSInstancing", "ps_5_1", pInclude, pShaderBlob);
	pInclude->Close(pData);

	delete pInclude;

	return ByteCode;
}

void InstancingShader::CreateShader(ID3D12Device * pDevice, ID3D12RootSignature * pRootSignature)
{
	m_PipelineStateCount = 1;
	m_PipelineStates = new MWRL ComPtr<ID3D12PipelineState>[m_PipelineStateCount];
	Shader::CreateShader(pDevice, pRootSignature);
}

void InstancingShader::CreateShaderVariables(ID3D12Device * pDevice, ID3D12GraphicsCommandList * pCommandList)
{
	UINT Stride = sizeof(VS_VB_INSTANCE);

	m_ObjectsCB = CreateBufferResource(pDevice, pCommandList, NULL
		, Stride * m_ObjectCount
		, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_ObjectsCB->Map(0, NULL, (void**)&m_MappedGameObjects);

	m_InstancingBufferView.BufferLocation = m_ObjectsCB->GetGPUVirtualAddress();
	m_InstancingBufferView.StrideInBytes = Stride;
	m_InstancingBufferView.SizeInBytes = Stride * m_ObjectCount;

}

void InstancingShader::UpdateShaderVariables(ID3D12GraphicsCommandList * pCommandList)
{
	for (int i = 0; i < m_Objects.size(); ++i)
	{
		m_MappedGameObjects[i].m_Color = (i%2)? XMFLOAT4(0.5f, 0.f, 0.f, 1.f) : XMFLOAT4(0.0f, 0.0f, 0.5f, 1.0f);
		XMStoreFloat4x4(&m_MappedGameObjects[i].m_Transform, XMMatrixTranspose(XMLoadFloat4x4(&m_Objects[i]->GetWorldTransform())));
	}
}

void InstancingShader::ReleaseShaderVariables()
{
	if(m_ObjectsCB.Get()) m_ObjectsCB->Unmap(0, NULL);
	m_ObjectsCB.Reset();
}

void InstancingShader::Render(ID3D12GraphicsCommandList * pCommandList, Camera * pCamera, float Interpolation)
{
	Shader::Render(pCommandList, pCamera);
	UpdateShaderVariables(pCommandList);
	m_Objects[0]->Render(pCommandList, pCamera, m_ObjectCount, m_InstancingBufferView);
}
