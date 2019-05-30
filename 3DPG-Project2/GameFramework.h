#pragma once
#include <type_traits>
#include <stack>
#include "Timer.h"
#include "Scene.h"

_3DGP_BEGIN_

class GameFramework
{

	static const UINT FRAME_COUNT{ 2 };

public:

	static GameFramework* Get();


	_3DGP_EXPLICIT_CHILD_OF_TEMPLATE_(StartingScene, Scene)
	void Run(const std::string&  Title, int Width, int Height)
	{
		if (m_Initialized) return;

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
				delete m_Scenes.top();
			}
			m_Scenes.pop();
		}

		m_Scenes.push(new NewScene);
		m_Scenes.top()->Init(m_Device, m_CommandList);
		ThrowIfFailed(m_CommandList->Close());

		ID3D12CommandList* pCommandLists[] = { m_CommandList };
		m_CommandQueue->ExecuteCommandLists(_countof(pCommandLists), pCommandLists);

		WaitForGPU();
		if (m_Scenes.top()) m_Scenes.top()->ReleaseUploadBuffers();
		m_Timer.Reset();
	}

	void EnableDebugMode();

	void ChangeSwapChain(); //To-Do - use way to get Window Size

	LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

private:

	GameFramework();
	~GameFramework();

	void GameLoop();

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

	bool						m_ConsoleAllocated	{ false };
	bool						m_Initialized		{ false };

	std::string					m_WndTitle			{ "3DGP Project" };
	int							m_WndClientWidth	{ 0 };
	int							m_WndClientHeight	{ 0 };

	MSG							m_Message			{};
	HWND						m_HWND				{ NULL };
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

	D3D12_VIEWPORT				m_Viewport;
	D3D12_RECT					m_ScissorRect;

};

_3DGP_END_