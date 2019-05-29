#pragma once
#include "Timer.h"
#include "Scene.h"
#include <stack>

_3DGP_BEGIN_

class GameFramework
{

	static const UINT FRAME_COUNT{ 2 };

public:

	static GameFramework* Get()
	{
		static GameFramework* Instance = new GameFramework;
		return Instance;
	}

	template<class NewScene>
	void ChangeScenes(BOOL popScene)
	{
		ThrowIfFailed(m_CommandList->Reset(m_CommandAllocator.Get(), NULL));

		if (popScene)
		{
			if (m_Scenes.top())
			{
				m_Scenes.top()->Destroy();
				delete m_Scenes.top();
			}
			m_Scenes.pop();
		}
			
		if (!m_Device) return;

		m_Scenes.push(new NewScene);	
		m_Scenes.top()->Init(m_Device.Get(), m_CommandList.Get());
		ThrowIfFailed(m_CommandList->Close());

		ID3D12CommandList* pCommandLists[] = { m_CommandList.Get() };
		m_CommandQueue->ExecuteCommandLists(_countof(pCommandLists), pCommandLists);

		WaitForGPU();
		if (m_Scenes.top()) m_Scenes.top()->ReleaseUploadBuffers();
		m_Timer.Reset();
	}

	template<class StartingScene>
	void Run(const std::string &Title, int Width, int Height)
	{
		if (m_Initialized) return;

		m_Initialized = true;
		InitWindow(Title, Width, Height);

		CreateD3D12Device();
		CreateCommandInterfaces();
		CreateSwapChain();

		CreateRTVDescriptorHeap();
		CreateDSVDescriptorHeap();

		CreateRenderTargetView();
		CreateDepthStencilView();

		ChangeScenes<StartingScene>(FALSE);
		MainLoop();
	}

	void MainLoop();
	void EnableDebugMode();
	void ChangeSwapChain(); //To-Do - use way to get Window Size

	LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

private:

	GameFramework();
	~GameFramework();

	void InitFramework();
	void DestroyFramework();

	void InitWindow(const std::string& title, int width, int height);

	void UpdateClientRect();

	void CreateD3D12Device();
	void CreateCommandInterfaces();
	void CreateSwapChain();
	void CreateRTVDescriptorHeap();
	void CreateDSVDescriptorHeap();
	void CreateRenderTargetView();
	void CreateDepthStencilView();

	void PopulateCommandList();
	void WaitForGPU();
	void MoveToNextFrame();
	void OnDestroy();

private:

	std::stack<Scene*>	m_Scenes;

	bool			m_ConsoleAllocated	{ false };
	bool			m_Initialized		{ false };

	int				m_Width				{ 0 };
	int				m_Height			{ 0 };

	MSG				m_Message			{};
	HDC				m_HDC				{ NULL };
	HWND			m_HWND				{ NULL };
	HINSTANCE		m_hInstance;

	Timer			m_Timer;

	WRL ComPtr<IDXGIFactory4>	m_Factory;
	WRL ComPtr<IDXGISwapChain3> m_SwapChain;
	WRL ComPtr<ID3D12Device>	m_Device;

	bool m_4XMSAA_Enabled		{ false };
	UINT m_4XMSAA_QualityLevels	{ 0 };

	WRL ComPtr<ID3D12Resource>			m_RenderTargets[FRAME_COUNT];
	WRL ComPtr<ID3D12DescriptorHeap>	m_RTVDescriptorHeap;
	UINT								m_RTVDescriptorIncrementSize	{ 0 };


	WRL ComPtr<ID3D12Resource>			m_DepthStencilBuffer;
	WRL ComPtr<ID3D12DescriptorHeap>	m_DSVDescriptorHeap;
	UINT								m_DSVDescriptorIncrementSize	{ 0 };

	WRL ComPtr<ID3D12CommandQueue>			m_CommandQueue;
	WRL ComPtr<ID3D12CommandAllocator>		m_CommandAllocator;
	WRL ComPtr<ID3D12GraphicsCommandList>	m_CommandList;

	WRL ComPtr<ID3D12PipelineState>		m_PipelineState;
	WRL ComPtr<ID3D12Fence>				m_Fence;
	UINT64								m_FenceValues[FRAME_COUNT];
	UINT								m_FrameIndex	{ 0 };
	HANDLE								m_FenceEvent	{ NULL };

#ifdef _DEBUG
	WRL ComPtr<ID3D12Debug>				m_DebugController;
#endif

	D3D12_VIEWPORT						m_Viewport;
	D3D12_RECT							m_ScissorRect;

};

_3DGP_END_

