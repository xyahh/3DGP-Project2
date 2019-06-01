#include "stdafx.h"
#include "ObjectsShader.h"

#include "RotatingObject.h"
#include "DiffusedCubeMesh.h"

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

D3D12_SHADER_BYTECODE ObjectsShader::CreateVertexShader(ID3DBlob ** pShaderBlob)
{
	D3D12_SHADER_BYTECODE ByteCode;
	*pShaderBlob = CompileShader(L"Shaders.hlsl", "VSDiffused", "vs_5_1");
	ByteCode.pShaderBytecode = (*pShaderBlob)->GetBufferPointer();
	ByteCode.BytecodeLength = (*pShaderBlob)->GetBufferSize();
	return ByteCode;
}

D3D12_SHADER_BYTECODE ObjectsShader::CreatePixelShader(ID3DBlob ** pShaderBlob)
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
	m_PipelineStates = new ID3D12PipelineState*[m_PipelineStateCount];
	Shader::CreateShader(pDevice, pRootSignature);
}

void ObjectsShader::BuildObjects(ID3D12Device * pDevice, ID3D12GraphicsCommandList * pCommandList)
{
	DiffusedCubeMesh* pCubeMesh = new DiffusedCubeMesh(pDevice, pCommandList, 12.f, 12.f, 12.f);

	int xObjects = 10;
	int yObjects = 10;
	int zObjects = 10;
	int i = 0;

	m_ObjectCount = (xObjects * 2 + 1) * (yObjects * 2 + 1) * (zObjects * 2 + 1);

	m_Objects = new GameObject*[m_ObjectCount];
	float fxPadding = 12.f * 2.5f;
	float fyPadding = 12.f * 2.5f;
	float fzPadding = 12.f * 2.5f;

	RotatingObject* pRotatingObject{ NULL };
	for (int x = -xObjects; x <= xObjects; ++x)
	{
		for (int y = -yObjects; y <= yObjects; ++y)
		{
			for (int z = -zObjects; z <= zObjects; ++z)
			{
				pRotatingObject = new RotatingObject;
				pRotatingObject->SetMesh(pCubeMesh);
				pRotatingObject->SetPosition(fxPadding * x, fyPadding * y, fzPadding * z);
				pRotatingObject->SetRotationAxis(XMFLOAT3(0.f, 1.f, 0.f));
				pRotatingObject->SetAngularSpeed(10.f * (i % 10) + 3.f);
				m_Objects[i++] = pRotatingObject;

			}
		}
	}

	CreateShaderVariables(pDevice, pCommandList);

}

void ObjectsShader::ReleaseUploadBuffers()
{
	if (m_Objects)
	{
		for (int i = 0; i < m_ObjectCount; ++i)
		{
			if (m_Objects[i]) m_Objects[i]->ReleaseUploadBuffers();
		}
	}
}

void ObjectsShader::Update(float DeltaTime)
{
	for (int i = 0; i < m_ObjectCount; ++i)
		m_Objects[i]->Update(DeltaTime);
}

void ObjectsShader::ReleaseObjects()
{
	if (m_Objects)
	{
		for (int i = 0; i < m_ObjectCount; ++i)
		{
			if (m_Objects[i]) delete m_Objects[i];
		}
		delete[] m_Objects;
	}
}

void ObjectsShader::Render(ID3D12GraphicsCommandList * pCommandList, Camera * pCamera, float Interpolation)
{
	Shader::Render(pCommandList, pCamera);
	for (int i = 0; i < m_ObjectCount; ++i)
		m_Objects[i]->Render(pCommandList, pCamera);
}
