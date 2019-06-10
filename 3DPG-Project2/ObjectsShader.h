#pragma once
#include "Shader.h"

_3DGP_BEGIN_

class GameObject;

class ObjectsShader : public Shader
{
public:
	ObjectsShader();
	virtual ~ObjectsShader();

	virtual D3D12_INPUT_LAYOUT_DESC		CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE		CreateVertexShader(MWRL ComPtr<ID3DBlob>*pShaderBlob);
	virtual D3D12_SHADER_BYTECODE		CreatePixelShader(MWRL ComPtr<ID3DBlob>* pShaderBlob);

	virtual void						CreateShader(ID3D12Device* pDevice, ID3D12RootSignature* pRootSignature);

	virtual void						BuildObjects(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual void						ReleaseUploadBuffers();
	virtual void						Update(float DeltaTime);
	virtual void						ReleaseObjects();

	virtual void						Render(ID3D12GraphicsCommandList* pCommandList, Camera* pCamera, float Interpolation);

protected:
	STD vector<GameObject*>	m_Objects;
};

_3DGP_END_

