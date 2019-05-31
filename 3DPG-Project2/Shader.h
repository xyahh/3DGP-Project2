#pragma once
#include "Camera.h"

struct CB_GAMEOBJECT_INFO
{
	DX XMFLOAT4X4 m_World;
};

_3DGP_BEGIN_

class Shader
{
public:
	Shader();
	virtual ~Shader();

	void AddRef() { ++m_ReferenceCount; }
	void Release() { if (--m_ReferenceCount <= 0) delete this; }

	virtual D3D12_INPUT_LAYOUT_DESC		CreateInputLayout();
	virtual D3D12_RASTERIZER_DESC		CreateRasterizerState();
	virtual D3D12_BLEND_DESC			CreateBlendState();
	virtual D3D12_DEPTH_STENCIL_DESC	CreateDepthStencilState();

	virtual D3D12_SHADER_BYTECODE		CreateVertexShader(ID3DBlob** pShaderBlob);
	virtual D3D12_SHADER_BYTECODE		CreatePixelShader(ID3DBlob** pShaderBlob);

	virtual void						CreateShader(ID3D12Device* pDevice, ID3D12RootSignature* pRootSignature);
	virtual void						CreateShaderVariables(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual void						UpdateShaderVariables(ID3D12GraphicsCommandList* pCommandList);
	virtual void						UpdateShaderVariable(ID3D12GraphicsCommandList* pCommandList, DX XMFLOAT4X4* pWorld);
	virtual void						ReleaseShaderVariables();

	virtual void						PreRender(ID3D12GraphicsCommandList* pCommandList);
	virtual	void						Render(ID3D12GraphicsCommandList* pCommandList, Camera* pCamera);

private:
	int m_ReferenceCount	{ 0 };

protected:		
	ID3D12PipelineState**	m_PipelineStates	{ NULL };
	int						m_PipelineStateCount{ 0 };
};


_3DGP_END_

