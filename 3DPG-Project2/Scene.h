#pragma once
#include "Shader.h"

_3DGP_BEGIN_

class Scene
{
public:
	Scene() {}
	virtual ~Scene() {}

	virtual void Init(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList) = 0;
	virtual void Destroy() = 0;

	virtual bool MouseMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;
	virtual bool KeyboardMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

	virtual void Render(ID3D12GraphicsCommandList* pCommandList, float Interpolation) = 0;
	virtual void Update(float DeltaTime) = 0;

	virtual void ReleaseUploadBuffers() = 0;
	

	WRL ComPtr<ID3D12RootSignature> CreateRootSignature(ID3D12Device* pDevice);
	ID3D12RootSignature* GetRootSignature() const;


protected:
	WRL ComPtr<ID3D12RootSignature>	m_RootSignature;
	WRL ComPtr<ID3D12PipelineState> m_PipelineState;

	Shader** m_Shaders		{ NULL };
	int		m_ShaderCount	{ 0 };

};

_3DGP_END_