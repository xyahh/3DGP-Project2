#pragma once
#include "ObjectsShader.h"

_3DGP_BEGIN_

struct VS_VB_INSTANCE
{
	DX XMFLOAT4X4	m_Transform;
	DX XMFLOAT4		m_Color;
};

class InstancingShader : public ObjectsShader
{
public:

	InstancingShader();

	virtual ~InstancingShader();

	virtual D3D12_INPUT_LAYOUT_DESC		CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE		CreateVertexShader(MWRL ComPtr<ID3DBlob>*pShaderBlob);
	virtual D3D12_SHADER_BYTECODE		CreatePixelShader(MWRL ComPtr<ID3DBlob>* pShaderBlob);

	virtual void						CreateShader(ID3D12Device* pDevice, ID3D12RootSignature* pRootSignature);

	virtual void						CreateShaderVariables(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual void						UpdateShaderVariables(ID3D12GraphicsCommandList* pCommandList);
	virtual void						ReleaseShaderVariables();

	virtual void						Render(ID3D12GraphicsCommandList* pCommandList, Camera* pCamera, float Interpolation);

protected:
	MWRL ComPtr<ID3D12Resource>		m_ObjectsCB;
	VS_VB_INSTANCE*					m_MappedGameObjects;

	D3D12_VERTEX_BUFFER_VIEW		m_InstancingBufferView;
};

_3DGP_END_

