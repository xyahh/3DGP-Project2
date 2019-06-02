#pragma once
#include <type_traits>
#include <stack>
#include "Timer.h"
#include "Scene.h"
#include "Camera.h"
#include "Player.h"

_3DGP_BEGIN_

class GameFramework
{

	static const UINT FRAME_COUNT{ 2 };

public:

	static GameFramework* Get();

	_3DGP_EXPLICIT_CHILD_OF_TEMPLATE_(StartingScene, Scene)
	void Run(const std::string&  Title, bool DebugMode=false, int Width = GetSystemMetrics(SM_CXSCREEN), int Height = GetSystemMetrics(SM_CYSCREEN))
	{
		if (m_Initialized) return;

		if (DebugMode)
			EnableDebugMode();
		else
			FreeConsole();

		m_Initialized = true;
		InitWindow(Title, Width, Height);
		InitDirectX();
		ChangeScenes<StartingScene>(FALSE);
		GameLoop();
	}

	_3DGP_EXPLICIT_CHILD_OF_TEMPLATE_(NewScene, Scene)
	void ChangeScenes(BOOL popScene)
	{
		if (!m_Initialized) return;
		ThrowIfFailed(m_CommandList->Reset(m_CommandAllocator, NULL));

		if (popScene)
		{
			if (m_Scenes.top())
			{
				m_Scenes.top()->Destroy();
				m_Player = NULL;
				m_Camera = NULL;
				delete m_Scenes.top();
			}
			m_Scenes.pop();
		}

		m_Scenes.push(new NewScene);
		m_Player = m_Scenes.top()->Init(m_Device, m_CommandList);
		m_Camera = m_Player->GetCamera();
		ThrowIfFailed(m_CommandList->Close());

		ID3D12CommandList* pCommandLists[] = { m_CommandList };
		m_CommandQueue->ExecuteCommandLists(_countof(pCommandLists), pCommandLists);

		WaitForGPU();
		if (m_Scenes.top()) m_Scenes.top()->ReleaseUploadBuffers();
		m_Timer.Reset();
	}

	void ToggleFullscreen(); //To-Do - use way to get Window Size

	void GetWindowSize(int* Width, int* Height) const;

	LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

	HWND GetWindowHandle() const;

private:

	GameFramework();
	~GameFramework();

	void GameLoop();

	void EnableDebugMode();

	void InitFramework();
	void InitDirectX();
	void DestroyFramework();

	void InitWindow(const std::string&  title, int width, int height);

	void UpdateClientRect();

	void CreateD3Device();
	void CreateCommandInterfaces();
	void CreateSwapChain();
	void CreateRTVDSVDescriptorHeaps();
	void CreateRenderTargetView();
	void CreateDepthStencilView();

	void PopulateCommandList();
	void WaitForGPU();
	void MoveToNextFrame();
	void OnDestroy();

private:

	std::stack<Scene*>			m_Scenes;

	bool						m_DebugModeEnabled	{ false };
	bool						m_Initialized		{ false };

	std::string					m_WndTitle			{ "3DGP Project" };
	int							m_WndClientWidth	{ 0 };
	int							m_WndClientHeight	{ 0 };

	MSG							m_Message			{};
	HWND						m_hWnd				{ NULL };
	HINSTANCE					m_hInstance;

	Timer						m_Timer;

	IDXGIFactory4*				m_Factory;
	IDXGISwapChain3*			m_SwapChain;
	ID3D12Device*				m_Device;

	bool						m_4XMSAA_Enabled		{ false };
	UINT						m_4XMSAA_QualityLevels	{ 0 };

	ID3D12Resource*				m_SwapChainBuffer[FRAME_COUNT];
	ID3D12DescriptorHeap*		m_RTVDescriptorHeap;
	UINT						m_RTVDescriptorIncrementSize	{ 0 };


	ID3D12Resource*				m_DepthStencilBuffer;
	ID3D12DescriptorHeap*		m_DSVDescriptorHeap;
	UINT						m_DSVDescriptorIncrementSize	{ 0 };

	ID3D12CommandQueue*			m_CommandQueue;
	ID3D12CommandAllocator*		m_CommandAllocator;
	ID3D12GraphicsCommandList*	m_CommandList;
	ID3D12Fence*				m_Fence;

	UINT64						m_FenceValues[FRAME_COUNT];
	UINT						m_FrameIndex	{ 0 };
	HANDLE						m_FenceEvent	{ NULL };

	Camera*						m_Camera		{ NULL };
	Player*						m_Player		{ NULL };
};

_3DGP_END_