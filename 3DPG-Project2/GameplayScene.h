#pragma once
#include "Scene.h"

_3DGP_BEGIN_

class GameplayScene :public Scene
{
public:
	GameplayScene();
	virtual ~GameplayScene();

	virtual void Init(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual void Destroy();

	virtual bool MouseMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual bool KeyboardMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual void Render(ID3D12GraphicsCommandList* pCommandList, float Interpolation);
	virtual void Update(float DeltaTime);

	virtual void ReleaseUploadBuffers();
};

_3DGP_END_

