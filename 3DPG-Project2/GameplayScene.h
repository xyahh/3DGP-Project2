#pragma once
#include "Scene.h"

_3DGP_BEGIN_

class GameplayScene :public Scene
{
public:
	GameplayScene();
	virtual ~GameplayScene();

	virtual Player* Init(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual void Destroy();

	virtual bool WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual bool ProcessInput();
	virtual void Render(ID3D12GraphicsCommandList* pCommandList, Camera* pCamera, float Interpolation);
	virtual void Update(float DeltaTime);

	virtual void ReleaseUploadBuffers();

private:
	POINT m_CursorPos;
	float m_CurrentTimeDilation	{ 1.f };
	float m_TargetTimeDilation	{ 1.f };
};

_3DGP_END_

