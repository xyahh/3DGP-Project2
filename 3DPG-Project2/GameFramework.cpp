#include "stdafx.h"
#include "GameFramework.h"
/* Dev Console */
#include <fcntl.h>
#include <io.h>

_3DGP_USE_
DX_USE

GameFramework::GameFramework()
{
	InitFramework();
}

GameFramework::~GameFramework()
{
	DestroyFramework();
}

void GameFramework::InitFramework()
{
	// No need to init ComPtrs to null as the ComPtr does this in its constructor
	m_Initialized = false;
	m_ConsoleAllocated = false;

	for (int i = 0; i < FRAME_COUNT; ++i)
		m_FenceValues[i] = 0;
}

void GameFramework::DestroyFramework()
{
	while (m_Scenes.size() > 0)
	{
		if (m_Scenes.top())
		{
			m_Scenes.top()->Destroy();
			delete m_Scenes.top();
		}
		m_Scenes.pop();
	}
}

void GameFramework::InitWindow(const std::string & title, int width, int height)
{
	EnableMenuItem(GetSystemMenu(GetConsoleWindow(), FALSE), SC_CLOSE, MF_GRAYED);

	m_Width = width;
	m_Height = height;

	RECT WindowRect{ 0, 0, width, height };
	WNDCLASS    wc;
	DWORD       dwExStyle;
	DWORD       dwStyle;

	LPCTSTR lpszClass = TEXT("3DGP-Project2");
	m_hInstance = GetModuleHandle(NULL);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = (WNDPROC)[](HWND h, UINT u, WPARAM w, LPARAM l)->LRESULT
	{ return GameFramework::Get()->WndProc(h, u, w, l); };
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hInstance;
	wc.hIcon = 0;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = lpszClass;
	RegisterClass(&wc);

	dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	//dwStyle = WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME;
	dwStyle = WS_OVERLAPPEDWINDOW;
	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);

	RECT ScreenSize;
	GetWindowRect(GetDesktopWindow(), &ScreenSize);

	POINT Center
	{
		(ScreenSize.right - WindowRect.right) / 2 ,
		(ScreenSize.bottom - WindowRect.bottom) / 2
	};

	// Create The Window
	m_HWND = CreateWindowEx
	(
		dwExStyle,
		lpszClass,
		title.c_str(),
		dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		Center.x, Center.y,
		WindowRect.right - WindowRect.left,
		WindowRect.bottom - WindowRect.top,
		NULL,
		NULL,
		m_hInstance,
		NULL
	);
	ShowWindow(m_HWND, SW_SHOW);
	SetForegroundWindow(m_HWND);
	SetFocus(m_HWND);
}

void GameFramework::UpdateClientRect()
{
	RECT ClientRect;
	::GetClientRect(m_HWND, &ClientRect);
	m_Width = ClientRect.right - ClientRect.left;
	m_Height = ClientRect.bottom - ClientRect.top;
}

void GameFramework::FrameworkLoop()
{
	while (m_Message.message != WM_QUIT)
	{

		if (PeekMessage(&m_Message, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&m_Message);
			DispatchMessage(&m_Message);
		}

		//Rendering & Update is done in this function
		PopulateCommandList();

		ID3D12CommandList* CommandLists[] = { m_CommandList.Get() };

		m_CommandQueue->ExecuteCommandLists(_countof(CommandLists), CommandLists);

		WaitForGPU();
		ThrowIfFailed(m_SwapChain->Present(1, 0));
		MoveToNextFrame();
	}
}

void GameFramework::GameLoop()
{
	m_Timer.Tick();
	while (m_Timer.FlushAccumulatedTime())
		m_Scenes.top()->Update(m_Timer.GetDeltaTime());
	m_Scenes.top()->Render(m_CommandList.Get(), m_Timer.Interpolation());
}

void GameFramework::EnableDebugMode()
{
	if (m_ConsoleAllocated) return;

	m_ConsoleAllocated = true;
	AllocConsole();

	SetConsoleTitle("3DGP Project 2 Console");
	typedef struct
	{
		char* _ptr;
		int _cnt;
		char* _base;
		int _flag;
		int _file;
		int _charbuf;
		int _bufsiz;
		char* _tmpfname;
	} FILE_COMPLETE;

	*(FILE_COMPLETE*)stdout = *(FILE_COMPLETE*)_fdopen(_open_osfhandle(PtrToLong(GetStdHandle(STD_OUTPUT_HANDLE)), _O_TEXT), "w");
	*(FILE_COMPLETE*)stderr = *(FILE_COMPLETE*)_fdopen(_open_osfhandle(PtrToLong(GetStdHandle(STD_ERROR_HANDLE)), _O_TEXT), "w");
	*(FILE_COMPLETE*)stdin = *(FILE_COMPLETE*)_fdopen(_open_osfhandle(PtrToLong(GetStdHandle(STD_INPUT_HANDLE)), _O_TEXT), "r");
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);
	setvbuf(stdin, NULL, _IONBF, 0);
}

void GameFramework::CreateD3D12Device()
{
	UINT DXGIFactoryFlags = 0;

#if defined(_DEBUG)
	{
		WRL ComPtr<ID3D12Debug> DebugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&DebugController))))
		{
			DebugController->EnableDebugLayer();
			DXGIFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
	}
#endif

	ThrowIfFailed(CreateDXGIFactory2(DXGIFactoryFlags, IID_PPV_ARGS(&m_Factory)));
	{
		WRL ComPtr<IDXGIAdapter1> Adapter;
		for (UINT i = 0; DXGI_ERROR_NOT_FOUND != m_Factory->EnumAdapters1(i, &Adapter); ++i)
		{
			DXGI_ADAPTER_DESC1 DXGIAdapaterDesc;
			Adapter->GetDesc1(&DXGIAdapaterDesc);
			if (DXGIAdapaterDesc.Flags && DXGI_ADAPTER_FLAG_SOFTWARE) continue; //only consider hardware adapters
			if (SUCCEEDED(D3D12CreateDevice(Adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_Device)))) break;
		}

		if (!Adapter)
		{
			m_Factory->EnumWarpAdapter(IID_PPV_ARGS(&Adapter));
			ThrowIfFailed(D3D12CreateDevice(Adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_Device)));
		}
	}

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS MSAAQualityLevels;
	MSAAQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	MSAAQualityLevels.SampleCount = 4;
	MSAAQualityLevels.NumQualityLevels = 0;
	MSAAQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;

	ThrowIfFailed(m_Device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &MSAAQualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS)));

	m_4XMSAA_QualityLevels = MSAAQualityLevels.NumQualityLevels;
	m_4XMSAA_Enabled = (m_4XMSAA_QualityLevels > 1) ? true : false;

	ThrowIfFailed(m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence)));
	m_FenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);

	m_Viewport.TopLeftX = 0;
	m_Viewport.TopLeftY = 0;
	m_Viewport.Width = static_cast<float>(m_Width);
	m_Viewport.Height = static_cast<float>(m_Height);
	m_Viewport.MinDepth = 0.f;
	m_Viewport.MaxDepth = 1.f;

	m_ScissorRect = { 0, 0, (LONG)m_Viewport.Width, (LONG)m_Viewport.Height };
}

void GameFramework::CreateCommandInterfaces()
{
	//Command Queue
	D3D12_COMMAND_QUEUE_DESC CommandQueueDesc;
	::ZeroMemory(&CommandQueueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));
	CommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	CommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	ThrowIfFailed(m_Device->CreateCommandQueue(&CommandQueueDesc, IID_PPV_ARGS(&m_CommandQueue)));

	//Command Allocator & List
	ThrowIfFailed(m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CommandAllocator)));
	ThrowIfFailed(m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAllocator.Get(), NULL, IID_PPV_ARGS(&m_CommandList)));

	//Start CommandList closed.
	m_CommandList->Close();
}

void GameFramework::CreateSwapChain()
{
	UpdateClientRect();
	
	DXGI_SWAP_CHAIN_DESC SwapChainDesc;
	::ZeroMemory(&SwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	SwapChainDesc.BufferDesc.Width = m_Width;
	SwapChainDesc.BufferDesc.Height = m_Height;
	SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	SwapChainDesc.SampleDesc.Count = (m_4XMSAA_Enabled) ? 4 : 1;
	SwapChainDesc.SampleDesc.Quality = (m_4XMSAA_Enabled) ? (m_4XMSAA_QualityLevels - 1) : 0;

	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDesc.BufferCount = FRAME_COUNT;

	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	SwapChainDesc.Windowed = TRUE;
	SwapChainDesc.OutputWindow = m_HWND;
	SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;


	ThrowIfFailed(m_Factory->CreateSwapChain(m_CommandQueue.Get(), &SwapChainDesc, (IDXGISwapChain**)m_SwapChain.GetAddressOf()));

	m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();
	ThrowIfFailed(m_Factory->MakeWindowAssociation(m_HWND, DXGI_MWA_NO_ALT_ENTER));

#ifndef _WITH_SWAPCHAIN_FULLSCREEN_STATE
	CreateRenderTargetView();
#endif

}

void GameFramework::CreateRTVDescriptorHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeapDesc;
	::ZeroMemory(&DescriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));

	DescriptorHeapDesc.NumDescriptors = FRAME_COUNT;
	DescriptorHeapDesc.NodeMask = 0;
	DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	DescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	ThrowIfFailed(m_Device->CreateDescriptorHeap(&DescriptorHeapDesc, IID_PPV_ARGS(&m_RTVDescriptorHeap)));

	m_RTVDescriptorIncrementSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
}

void GameFramework::CreateDSVDescriptorHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeapDesc;
	::ZeroMemory(&DescriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));

	DescriptorHeapDesc.NumDescriptors = 1;
	DescriptorHeapDesc.NodeMask = 0;
	DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	DescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	ThrowIfFailed(m_Device->CreateDescriptorHeap(&DescriptorHeapDesc, IID_PPV_ARGS(&m_DSVDescriptorHeap)));

	m_DSVDescriptorIncrementSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

void GameFramework::CreateRenderTargetView()
{
	D3D12_CPU_DESCRIPTOR_HANDLE RTVCPUDescriptorHandle = m_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	for (UINT i = 0; i < FRAME_COUNT; ++i)
	{
		ThrowIfFailed(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_RenderTargets[i])));
		m_Device->CreateRenderTargetView(m_RenderTargets[i].Get(), NULL, RTVCPUDescriptorHandle);
		RTVCPUDescriptorHandle.ptr += m_RTVDescriptorIncrementSize;
	}
}

void GameFramework::CreateDepthStencilView()
{
	D3D12_RESOURCE_DESC ResourceDesc;

	ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	ResourceDesc.Alignment = 0;
	ResourceDesc.Width = m_Width;
	ResourceDesc.Height = m_Height;
	ResourceDesc.DepthOrArraySize = 1;

	ResourceDesc.MipLevels = 1;
	ResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	ResourceDesc.SampleDesc.Count	= (m_4XMSAA_Enabled) ? 4 : 1;
	ResourceDesc.SampleDesc.Quality = (m_4XMSAA_Enabled) ? (m_4XMSAA_QualityLevels - 1) : 0;

	ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	ResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES HeapProperties;
	::ZeroMemory(&HeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
	HeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	HeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	HeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	HeapProperties.VisibleNodeMask = 1;
	HeapProperties.CreationNodeMask = 1;

	D3D12_CLEAR_VALUE ClearValue;
	ClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	ClearValue.DepthStencil.Depth = 1.f;
	ClearValue.DepthStencil.Stencil = 0;

	ThrowIfFailed(m_Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, 
		&ClearValue, IID_PPV_ARGS(&m_DepthStencilBuffer)));

	D3D12_CPU_DESCRIPTOR_HANDLE DSVCPUDescriptorHandle = m_DSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_Device->CreateDepthStencilView(m_DepthStencilBuffer.Get(), NULL, DSVCPUDescriptorHandle);
}

void GameFramework::PopulateCommandList()
{
	ThrowIfFailed(m_CommandAllocator->Reset());
	ThrowIfFailed(m_CommandList->Reset(m_CommandAllocator.Get(), m_PipelineState.Get()));

	D3D12_RESOURCE_BARRIER ResourceBarrier;
	::ZeroMemory(&ResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
	ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	ResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	ResourceBarrier.Transition.pResource = m_RenderTargets[m_FrameIndex].Get();
	ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	ResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	m_CommandList->ResourceBarrier(1, &ResourceBarrier);	
	m_CommandList->RSSetViewports(1, &m_Viewport);
	m_CommandList->RSSetScissorRects(1, &m_ScissorRect);

	D3D12_CPU_DESCRIPTOR_HANDLE RTVDescriptorHandle = m_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	RTVDescriptorHandle.ptr += m_FrameIndex * m_RTVDescriptorIncrementSize;

	float ClearColor[4] = { 0.f, 0.25f, 0.f, 1.f };
	m_CommandList->ClearRenderTargetView(RTVDescriptorHandle, ClearColor, 0, NULL);

	D3D12_CPU_DESCRIPTOR_HANDLE DSVDescriptorHandle = m_DSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_CommandList->ClearDepthStencilView(DSVDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.f, 0, 0, NULL);

	m_CommandList->OMSetRenderTargets(1, &RTVDescriptorHandle, TRUE, &DSVDescriptorHandle);
	
	/* Game Loop */
	//Render here before closing the Command List and before transitioning the Resource State to present	
	GameLoop();

	ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	ResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	m_CommandList->ResourceBarrier(1, &ResourceBarrier);

	ThrowIfFailed(m_CommandList->Close());
}

void GameFramework::ChangeSwapChain()
{
	WaitForGPU();

	BOOL FullScreenState = FALSE;
	m_SwapChain->GetFullscreenState(&FullScreenState, NULL);
	m_SwapChain->SetFullscreenState(!FullScreenState, NULL);

	DXGI_MODE_DESC TargetParameters;
	TargetParameters.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	TargetParameters.Height = m_Height;
	TargetParameters.Width = m_Width;
	TargetParameters.RefreshRate.Denominator = 1;
	TargetParameters.RefreshRate.Numerator = 60;
	TargetParameters.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	TargetParameters.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	
	m_SwapChain->ResizeTarget(&TargetParameters);
	 
	for (int i = 0; i < FRAME_COUNT; ++i)
		m_RenderTargets[i].Reset();

	DXGI_SWAP_CHAIN_DESC SwapChainDesc;
	m_SwapChain->GetDesc(&SwapChainDesc);
	m_SwapChain->ResizeBuffers(FRAME_COUNT, m_Width, m_Height, SwapChainDesc.BufferDesc.Format, SwapChainDesc.Flags);

	m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();

	CreateRenderTargetView();

}

void GameFramework::WaitForGPU()
{
	const UINT64 Fence = ++m_FenceValues[m_FrameIndex];
	ThrowIfFailed(m_CommandQueue->Signal(m_Fence.Get(), Fence));

	if (m_Fence->GetCompletedValue() < Fence)
	{
		ThrowIfFailed(m_Fence->SetEventOnCompletion(Fence, m_FenceEvent));
		::WaitForSingleObject(m_FenceEvent, INFINITE);
	}

	m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();
}

void GameFramework::MoveToNextFrame()
{
	m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();

	UINT64 FenceValue = ++m_FenceValues[m_FrameIndex];
	ThrowIfFailed(m_CommandQueue->Signal(m_Fence.Get(), FenceValue));
	if (m_Fence->GetCompletedValue() < FenceValue)
	{
		ThrowIfFailed(m_Fence->SetEventOnCompletion(FenceValue, m_FenceEvent));
		::WaitForSingleObject(m_FenceEvent, INFINITE);
	}
}

void GameFramework::OnDestroy()
{
	WaitForGPU(); // Do not destroy Handle before clearing the Command Queue
	::CloseHandle(m_FenceEvent);
	m_SwapChain->SetFullscreenState(FALSE, NULL);
}

LRESULT GameFramework::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return FALSE;
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_RETURN:
			GameFramework::Get()->ChangeSwapChain();
			break;
		}
		break;
	case WM_SIZE:

		return FALSE;
	}
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
