#pragma once
#include "ObjectsShader.h"
#include "Player.h"
#include "Camera.h"

_3DGP_BEGIN_

class Scene
{
public:
	Scene() {}
	virtual ~Scene() {}

	/*
	The Init Builds all the Objects and ultimately returns the Main Player
	of the Scene created.
	*/
	virtual Player* Init(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList) = 0;
	virtual void Destroy() = 0;

	virtual bool WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

	virtual void ProcessInput() = 0;
	virtual void Render(ID3D12GraphicsCommandList* pCommandList, Camera* pCamera, float Interpolation) = 0;
	virtual void Update(float DeltaTime) = 0;

	virtual void ReleaseUploadBuffers() = 0;
	

	MWRL ComPtr<ID3D12RootSignature> CreateRootSignature(ID3D12Device* pDevice);
	ID3D12RootSignature* GetRootSignature() const;


protected:
	
	MWRL ComPtr<ID3D12RootSignature>	m_RootSignature;
	STD vector<ObjectShader*>			m_ObjectShaders;
};

_3DGP_END_