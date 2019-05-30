#pragma once
#include "GameObject.h"

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

	virtual void						CreateShader(ID3D12Device* pDevice, ID3D12RootSignature* pRootSignature);
	virtual void						CreateShaderVariables(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual void						UpdateShaderVariables(ID3D12GraphicsCommandList* pCommandList);
	virtual void						ReleaseShaderVariables();

	virtual void						ReleaseUploadBuffers();
	virtual void						BuildObjects(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList, void* pContext = NULL);
	virtual void						ReleaseObjects();


	virtual void						Update(float DeltaTime);
	virtual void						PreRender(ID3D12GraphicsCommandList* pCommandList);
	virtual	void						Render(ID3D12GraphicsCommandList* pCommandList);

private:
	int m_ReferenceCount{ 0 };

protected:
	GameObject**	m_Objects{ NULL };
	int				m_ObjectCount{ 0 };
		
	ID3D12PipelineState**	m_PipelineStates;
	int						m_PipelineStateCount{ 0 };
};


_3DGP_END_

