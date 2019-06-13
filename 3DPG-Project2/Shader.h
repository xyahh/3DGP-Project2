#pragma once

_3DGP_BEGIN_

class Camera;

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

	D3D12_SHADER_BYTECODE				CompileShaderFromFile(const STD wstring& filename, const STD string& entrypoint, const STD string& target, ID3DInclude* pInclude, MWRL ComPtr<ID3DBlob>* pShaderBlob);

	virtual D3D12_SHADER_BYTECODE		CreateVertexShader(MWRL ComPtr<ID3DBlob>* pShaderBlob);
	virtual D3D12_SHADER_BYTECODE		CreatePixelShader(MWRL ComPtr<ID3DBlob>* pShaderBlob);

	virtual void						CreateShader(ID3D12Device* pDevice, ID3D12RootSignature* pRootSignature);
	virtual void						CreateShaderVariables(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual void						UpdateShaderVariables(ID3D12GraphicsCommandList* pCommandList);
	virtual void						ReleaseShaderVariables();

	virtual	void						Render(ID3D12GraphicsCommandList* pCommandList, Camera* pCamera);

private:

	int m_ReferenceCount	{ 0 };

protected:		

	MWRL ComPtr<ID3D12PipelineState>*	m_PipelineStates		{ NULL };
	int									m_PipelineStateCount	{ 0 };
};


_3DGP_END_

