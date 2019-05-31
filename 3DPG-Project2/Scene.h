#pragma once
#include "Camera.h"
#include "GameObject.h"

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

	virtual void Render(ID3D12GraphicsCommandList* pCommandList, Camera* pCamera, float Interpolation) = 0;
	virtual void Update(float DeltaTime) = 0;

	virtual void ReleaseUploadBuffers() = 0;
	

	ID3D12RootSignature* CreateRootSignature(ID3D12Device* pDevice);
	ID3D12RootSignature* GetRootSignature() const;


protected:

	ID3D12RootSignature*	m_RootSignature{ NULL };
	GameObject**			m_Objects{ NULL };
	int						m_ObjectCount{ 0 };

};

_3DGP_END_