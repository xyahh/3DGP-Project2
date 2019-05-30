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
		for (int i = 0; i < m_PipelineStateCount; ++i)
			if(m_PipelineStates[i]) m_PipelineStates[i]->Release();
		delete[] m_PipelineStates; 
		m_PipelineStateCount = 0;
	}
}

D3D12_INPUT_LAYOUT_DESC Shader::CreateInputLayout()
{
	UINT InputElementCount = 2;
	D3D12_INPUT_ELEMENT_DESC* pInputElementDesc = new D3D12_INPUT_ELEMENT_DESC[InputElementCount];

	pInputElementDesc[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pInputElementDesc[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC InputLayoutDesc;
	InputLayoutDesc.pInputElementDescs = pInputElementDesc;
	InputLayoutDesc.NumElements = InputElementCount;

	return InputLayoutDesc;
}

D3D12_RASTERIZER_DESC Shader::CreateRasterizerState()
{
	D3D12_RASTERIZER_DESC RasterizerDesc;
	::ZeroMemory(&RasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
	RasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	RasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	RasterizerDesc.FrontCounterClockwise = FALSE;
	RasterizerDesc.DepthBias = 0;
	RasterizerDesc.DepthBiasClamp = 0.0f;
	RasterizerDesc.SlopeScaledDepthBias = 0.0f;
	RasterizerDesc.DepthClipEnable = TRUE;
	RasterizerDesc.MultisampleEnable = FALSE;
	RasterizerDesc.AntialiasedLineEnable = FALSE;
	RasterizerDesc.ForcedSampleCount = 0;
	RasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return RasterizerDesc;
}
		
D3D12_BLEND_DESC Shader::CreateBlendState()
{
	D3D12_BLEND_DESC BlendDesc;
	::ZeroMemory(&BlendDesc, sizeof(D3D12_BLEND_DESC));
	BlendDesc.AlphaToCoverageEnable = FALSE;
	BlendDesc.IndependentBlendEnable = FALSE;
	BlendDesc.RenderTarget[0].BlendEnable = FALSE;
	BlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	BlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	BlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	BlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	BlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	BlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	return BlendDesc;
}

D3D12_DEPTH_STENCIL_DESC Shader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC DepthStencilDesc;
	::ZeroMemory(&DepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));

	DepthStencilDesc.DepthEnable = TRUE;
	DepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	DepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	DepthStencilDesc.StencilEnable = FALSE;
	DepthStencilDesc.StencilReadMask = 0x00;
	DepthStencilDesc.StencilWriteMask = 0x00;
	DepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	DepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	DepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	DepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	DepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	DepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	DepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	DepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return DepthStencilDesc;
}

void Shader::CreateShader(ID3D12Device * pDevice, ID3D12RootSignature * pRootSignature)
{
	m_PipelineStateCount = 1;
	m_PipelineStates = new ID3D12PipelineState*[m_PipelineStateCount];

	ID3DBlob* pVertexShaderBlob = NULL;
	ID3DBlob* pPixelShaderBlob = NULL;
	pVertexShaderBlob = CompileShader(L"Shaders.hlsl", "VSMain", "vs_5_1");
	pPixelShaderBlob = CompileShader(L"Shaders.hlsl", "PSMain", "ps_5_1");

	D3D12_GRAPHICS_PIPELINE_STATE_DESC PipelineStateDesc;
	::ZeroMemory(&PipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	PipelineStateDesc.pRootSignature = pRootSignature;

	PipelineStateDesc.VS.pShaderBytecode = pVertexShaderBlob->GetBufferPointer();
	PipelineStateDesc.VS.BytecodeLength = pVertexShaderBlob->GetBufferSize();

	PipelineStateDesc.PS.pShaderBytecode = pPixelShaderBlob->GetBufferPointer();
	PipelineStateDesc.PS.BytecodeLength = pPixelShaderBlob->GetBufferSize();

	PipelineStateDesc.RasterizerState	= CreateRasterizerState();
	PipelineStateDesc.BlendState		= CreateBlendState();
	PipelineStateDesc.DepthStencilState = CreateDepthStencilState();
	PipelineStateDesc.InputLayout		= CreateInputLayout();

	PipelineStateDesc.SampleMask = UINT_MAX;
	PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	PipelineStateDesc.NumRenderTargets = 1;

	PipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	PipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	PipelineStateDesc.SampleDesc.Count = 1;

	ThrowIfFailed(pDevice->CreateGraphicsPipelineState(&PipelineStateDesc,
		__uuidof(ID3D12PipelineState), (void **)&m_PipelineStates[0]));
	
	if (pVertexShaderBlob) pVertexShaderBlob->Release();
	if (pPixelShaderBlob) pPixelShaderBlob->Release();

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
	TriangleMesh *pTriangleMesh = new TriangleMesh(pDevice, pCommandList);

	m_ObjectCount = 1;

	m_Objects = new GameObject*[m_ObjectCount];

	m_Objects[0] = new GameObject();
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
	pCommandList->SetPipelineState(m_PipelineStates[0]);
}

void Shader::Render(ID3D12GraphicsCommandList * pCommandList)
{
	PreRender(pCommandList);
	for (int i = 0; i < m_ObjectCount; ++i)
		if (m_Objects[i])  m_Objects[i]->Render(pCommandList);

}
