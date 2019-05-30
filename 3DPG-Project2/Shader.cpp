#include "stdafx.h"
#include "Shader.h"
#include "TriangleMesh.h"

_3DGP_USE_
DX_USE

Shader::Shader()
{
}

Shader::~Shader()
{
	if (m_PipelineStates)
	{
		//when comptrs are deleted, they automatically release the Interface
		delete[] m_PipelineStates; 
		m_PipelineStateCount = 0;
	}
}

D3D12_INPUT_LAYOUT_DESC Shader::CreateInputLayout()
{
	UINT InputElementDescCount = 2;

	//this is deleted when Creating the Shader
	D3D12_INPUT_ELEMENT_DESC* InputElementDesc = new D3D12_INPUT_ELEMENT_DESC[InputElementDescCount];

	//12 bytes step cause of R32G32B32 = 96 bits / 8 = 12 bytes
	InputElementDesc[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	InputElementDesc[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC Desc;
	Desc.pInputElementDescs = InputElementDesc;
	Desc.NumElements = InputElementDescCount;

	return Desc;
}

D3D12_RASTERIZER_DESC Shader::CreateRasterizerState()
{
	D3D12_RASTERIZER_DESC Desc;
	::ZeroMemory(&Desc, sizeof(D3D12_RASTERIZER_DESC));
	Desc.FillMode = D3D12_FILL_MODE_SOLID;
	Desc.CullMode = D3D12_CULL_MODE_BACK;
	Desc.FrontCounterClockwise = FALSE;
	Desc.DepthBias = 0;
	Desc.DepthBiasClamp = 0.f;
	Desc.SlopeScaledDepthBias = 0.f;
	Desc.DepthClipEnable = TRUE;
	Desc.MultisampleEnable = FALSE;
	Desc.AntialiasedLineEnable = FALSE;
	Desc.ForcedSampleCount = 0;
	Desc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return Desc;
}
		
D3D12_BLEND_DESC Shader::CreateBlendState()
{
	D3D12_BLEND_DESC Desc;
	::ZeroMemory(&Desc, sizeof(D3D12_BLEND_DESC));

	Desc.AlphaToCoverageEnable = FALSE;
	Desc.IndependentBlendEnable = FALSE;
	Desc.RenderTarget[0].BlendEnable = FALSE;	

	Desc.RenderTarget[0].LogicOpEnable = FALSE;
	Desc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;

	Desc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	Desc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	Desc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;

	Desc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	Desc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	Desc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;

	Desc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return Desc;
}

D3D12_DEPTH_STENCIL_DESC Shader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC Desc;
	::ZeroMemory(&Desc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	Desc.DepthEnable = TRUE;
	Desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	Desc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	Desc.StencilEnable = FALSE;
	Desc.StencilReadMask = 0x00;
	Desc.StencilWriteMask = 0x00;

	Desc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	Desc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	Desc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	Desc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	Desc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	Desc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	Desc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	Desc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return Desc;
}

D3D12_SHADER_BYTECODE Shader::CreateVertexShader(ID3DBlob ** ppShaderBlob)
{
	assert(ppShaderBlob != nullptr);
	WRL ComPtr<ID3DBlob> ByteCode = CompileShader(L"Shaders.hlsl", NULL, "VSMain", "vs_5_1");
	*ppShaderBlob = ByteCode.Detach();

	D3D12_SHADER_BYTECODE ByteCodeDesc;
	ByteCodeDesc.BytecodeLength = (*ppShaderBlob)->GetBufferSize();
	ByteCodeDesc.pShaderBytecode = (*ppShaderBlob)->GetBufferPointer();
	return ByteCodeDesc;
}

D3D12_SHADER_BYTECODE Shader::CreatePixelShader(ID3DBlob ** ppShaderBlob)
{
	assert(ppShaderBlob != nullptr);
	WRL ComPtr<ID3DBlob> ByteCode = CompileShader(L"Shaders.hlsl", NULL, "PSMain", "ps_5_1");
	*ppShaderBlob = ByteCode.Detach();

	D3D12_SHADER_BYTECODE ByteCodeDesc;
	ByteCodeDesc.BytecodeLength = (*ppShaderBlob)->GetBufferSize();
	ByteCodeDesc.pShaderBytecode = (*ppShaderBlob)->GetBufferPointer();
	return ByteCodeDesc;
}

void Shader::CreateShader(ID3D12Device * pDevice, ID3D12RootSignature * pRootSignature)
{
	m_PipelineStateCount = 1;
	m_PipelineStates = new WRL ComPtr<ID3D12PipelineState>[m_PipelineStateCount];

	WRL ComPtr<ID3DBlob> pVertexShaderBlob;
	WRL ComPtr<ID3DBlob> pPixelShaderBlob;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC PipelineStateDesc;
	::ZeroMemory(&PipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	PipelineStateDesc.pRootSignature = pRootSignature;

	PipelineStateDesc.VS = CreateVertexShader(pVertexShaderBlob.GetAddressOf());
	PipelineStateDesc.PS = CreatePixelShader(pPixelShaderBlob.GetAddressOf());

	PipelineStateDesc.RasterizerState = CreateRasterizerState();
	PipelineStateDesc.BlendState = CreateBlendState();
	PipelineStateDesc.DepthStencilState = CreateDepthStencilState();
	PipelineStateDesc.InputLayout = CreateInputLayout();

	PipelineStateDesc.SampleMask = UINT_MAX;
	PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	PipelineStateDesc.NumRenderTargets = 1;

	PipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	PipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	PipelineStateDesc.SampleDesc.Count = 1;
	PipelineStateDesc.SampleDesc.Quality = 0;

	ThrowIfFailed(pDevice->CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(&m_PipelineStates[0])));

	//delete the InputElements created in CreateInputLayout() function
	if (PipelineStateDesc.InputLayout.pInputElementDescs) 
		delete[] PipelineStateDesc.InputLayout.pInputElementDescs;

}

void Shader::CreateShaderVariables(ID3D12Device * pDevice, ID3D12GraphicsCommandList * pCommandList)
{
}

void Shader::UpdateShaderVariables(ID3D12GraphicsCommandList * pCommandList)
{
}

void Shader::ReleaseShaderVariables()
{
}

void Shader::ReleaseUploadBuffers()
{
	if (m_Objects)
	{
		for (int i = 0; i < m_ObjectCount; ++i)
		{
			if (m_Objects[i]) m_Objects[i]->ReleaseUploadBuffers();	
		}
	}
}

void Shader::BuildObjects(ID3D12Device * pDevice, ID3D12GraphicsCommandList * pCommandList, void * pContext)
{
	TriangleMesh* pTriangleMesh = new TriangleMesh(pDevice, pCommandList);
	m_ObjectCount = 1;
	m_Objects = new GameObject*[m_ObjectCount];

	m_Objects[0] = new GameObject;
	m_Objects[0]->SetMesh(pTriangleMesh);

}

void Shader::ReleaseObjects()
{
	if (m_Objects)
	{
		for (int i = 0; i < m_ObjectCount; ++i)
		{
			if(m_Objects[i]) delete m_Objects[i];
		}
		delete[] m_Objects;
	}
}

void Shader::Update(float DeltaTime)
{
	for (int i = 0; i < m_ObjectCount; ++i)
		if(m_Objects[i]) m_Objects[i]->Update(DeltaTime);
}

void Shader::PreRender(ID3D12GraphicsCommandList * pCommandList)
{
	pCommandList->SetPipelineState(m_PipelineStates[0].Get());
}

void Shader::Render(ID3D12GraphicsCommandList * pCommandList)
{
	PreRender(pCommandList);
	for (int i = 0; i < m_ObjectCount; ++i)
		if (m_Objects[i])  m_Objects[i]->Render(pCommandList);

}
