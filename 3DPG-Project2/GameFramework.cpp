#include "stdafx.h"
#include "GameFramework.h"

/* Dev Console */
#include <fcntl.h>
#include <io.h>

_3DGP_USE_
DX_USE

GameFramework * GameFramework::Get()
{
	static GameFramework* Instance = new GameFramework;
	return Instance;
}

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
	m_Initialized = false;
	m_DebugModeEnabled = false;

	m_FrameIndex = 0;

	m_RTVDescriptorIncrementSize = 0;
	m_DSVDescriptorIncrementSize = 0;

	m_FenceEvent = NULL;
	for (int i = 0; i < FRAME_COUNT; ++i)
		m_FenceValues[i] = 0;
}

void GameFramework::InitDirectX()
{
	CreateD3Device();
	CreateCommandInterfaces();

	CreateRTVDSVDescriptorHeaps();

	CreateSwapChain();
	CreateDepthStencilView();

#ifdef _WITH_SWAPCHAIN_FULLSCREEN_STATE
	ChangeSwapChain();
#else
	CreateRenderTargetView();
#endif
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
	::CloseHandle(m_FenceEvent);
	m_SwapChain->SetFullscreenState(FALSE, NULL);
	if (m_Camera) delete m_Camera;
}

void GameFramework::InitWindow(const STD string& title, int width, int height)
{
	m_WndTitle = title;
	m_WndClientWidth = width;
	m_WndClientHeight = height;

	RECT WindowRect{ 0, 0, m_WndClientWidth, m_WndClientHeight };
	WNDCLASS    wc;
	DWORD       dwStyle;

	LPCTSTR lpszClass = TEXT("3DGP-Project2");

	m_hInstance = GetModuleHandle(NULL);

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)[](HWND h, UINT u, WPARAM w, LPARAM l)->LRESULT{ return GameFramework::Get()->WndProc(h, u, w, l); };
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hInstance;
	wc.hIcon = 0;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = lpszClass;

	RegisterClass(&wc);

	dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_BORDER;
	AdjustWindowRect(&WindowRect, dwStyle, FALSE);

	RECT ScreenSize;
	GetWindowRect(GetDesktopWindow(), &ScreenSize);

	POINT Center
	{
		(ScreenSize.right - WindowRect.right) / 2 ,
		(ScreenSize.bottom - WindowRect.bottom) / 2
	};

	m_hWnd = CreateWindow(lpszClass, m_WndTitle.c_str(), dwStyle, Center.x, Center.y,
		WindowRect.right - WindowRect.left,
		WindowRect.bottom - WindowRect.top,
		NULL, NULL, m_hInstance, NULL);

	ShowWindow(m_hWnd, SW_SHOW);
	UpdateWindow(m_hWnd);
}

void GameFramework::UpdateClientRect()
{
	RECT ClientRect;
	::GetClientRect(m_hWnd, &ClientRect);
	m_WndClientWidth = ClientRect.right - ClientRect.left;
	m_WndClientHeight = ClientRect.bottom - ClientRect.top;
}

void GameFramework::GameLoop()
{
	while (m_Message.message != WM_QUIT)
	{
		if (PeekMessage(&m_Message, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&m_Message);
			DispatchMessage(&m_Message);
			continue;
		}


		m_Timer.Tick();

		while (m_Timer.FlushAccumulatedTime())
		{
			m_Scenes.top()->ProcessInput();
			m_Scenes.top()->Update(m_Timer.GetDeltaTime());
		}

		m_Camera = m_Player->GetCamera();

		//Render is called in this function
		PopulateCommandList();

		ID3D12CommandList* CommandLists[] = { m_CommandList.Get() };

		m_CommandQueue->ExecuteCommandLists(_countof(CommandLists), CommandLists);

		WaitForGPU();

#ifdef _WITH_SYNCH_VERTICAL_RETRACE
		ThrowIfFailed(m_SwapChain->Present(1, 0));
#else
		ThrowIfFailed(m_SwapChain->Present(0, 0));
#endif
		MoveToNextFrame();
		::SetWindowText(m_hWnd, (m_WndTitle + " (FPS: " + m_Timer.GetFPS() + ")").c_str());
	}
}

void GameFramework::EnableDebugMode()
{
	/*  Do not allow debug mode after everything has been initialized.
		Enabling the debug layer after creating the ID3D12 Device will
		remove the device.
		*/
	if (m_DebugModeEnabled || m_Initialized) return;

	m_DebugModeEnabled = true;

	//Console
	{
		AllocConsole();
		SetConsoleTitle("3DGP Console");
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

		EnableMenuItem(GetSystemMenu(GetConsoleWindow(), FALSE), SC_CLOSE, MF_GRAYED);
	}
	
	//Debug Controller
	{
#if defined(_DEBUG)
	    MWRL ComPtr<ID3D12Debug> pDebugController;
		ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(pDebugController.GetAddressOf())));
		if (pDebugController)
			pDebugController->EnableDebugLayer();
#endif
	}
}

void GameFramework::CreateD3Device()
{
	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(m_Factory.GetAddressOf())));

	//Create Device
	{
		MWRL ComPtr<IDXGIAdapter1> pAdapter;

		for (UINT i = 0; DXGI_ERROR_NOT_FOUND != m_Factory->EnumAdapters1(i, &pAdapter); ++i)
		{
			DXGI_ADAPTER_DESC1 AdapterDesc;
			pAdapter->GetDesc1(&AdapterDesc);
			if (AdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;
			if (SUCCEEDED(D3D12CreateDevice(pAdapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_Device)))) break;
		}

		if (!pAdapter)
		{
			m_Factory->EnumWarpAdapter(IID_PPV_ARGS(&m_Factory));
			ThrowIfFailed(D3D12CreateDevice(pAdapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_Device)));
		}
	}
	
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS MSAAQualityLevels;
	MSAAQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	MSAAQualityLevels.SampleCount = 4;
	MSAAQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	MSAAQualityLevels.NumQualityLevels = 0;
	ThrowIfFailed(m_Device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &MSAAQualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS)));
	m_4XMSAA_QualityLevels = MSAAQualityLevels.NumQualityLevels;
	m_4XMSAA_Enabled = (m_4XMSAA_QualityLevels > 1) ? true : false;

	ThrowIfFailed(m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&m_Fence)));
	for (UINT i = 0; i < FRAME_COUNT; ++i) m_FenceValues[i] = 0;

	m_FenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
}

void GameFramework::CreateCommandInterfaces()
{
	D3D12_COMMAND_QUEUE_DESC CommandQueueDesc;
	::ZeroMemory(&CommandQueueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));
	CommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	CommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	ThrowIfFailed(m_Device->CreateCommandQueue(&CommandQueueDesc, IID_PPV_ARGS(&m_CommandQueue)));

	ThrowIfFailed(m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CommandAllocator)));

	ThrowIfFailed(m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAllocator.Get(), NULL, IID_PPV_ARGS(&m_CommandList)));
	ThrowIfFailed(m_CommandList->Close());
}

void GameFramework::CreateSwapChain()
{
	UpdateClientRect();
	
	DXGI_SWAP_CHAIN_DESC SwapChainDesc;
	::ZeroMemory(&SwapChainDesc, sizeof(SwapChainDesc));
	SwapChainDesc.BufferCount = FRAME_COUNT;
	SwapChainDesc.BufferDesc.Width = m_WndClientWidth;
	SwapChainDesc.BufferDesc.Height = m_WndClientHeight;
	SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	SwapChainDesc.OutputWindow = m_hWnd;
	SwapChainDesc.SampleDesc.Count = (m_4XMSAA_Enabled) ? 4 : 1;
	SwapChainDesc.SampleDesc.Quality = (m_4XMSAA_QualityLevels) ? (m_4XMSAA_QualityLevels - 1) : 0;
	SwapChainDesc.Windowed = TRUE;
	SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	ThrowIfFailed(m_Factory->CreateSwapChain(m_CommandQueue.Get(), &SwapChainDesc, (IDXGISwapChain **)m_SwapChain.GetAddressOf()));

	m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();

	ThrowIfFailed(m_Factory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER));	
}

void GameFramework::CreateRTVDSVDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeapDesc;
	::ZeroMemory(&DescriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	DescriptorHeapDesc.NumDescriptors = FRAME_COUNT;
	DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	DescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	DescriptorHeapDesc.NodeMask = 0;

	ThrowIfFailed(m_Device->CreateDescriptorHeap(&DescriptorHeapDesc, IID_PPV_ARGS(&m_RTVDescriptorHeap)));
	m_RTVDescriptorIncrementSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	DescriptorHeapDesc.NumDescriptors = 1;
	DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

	ThrowIfFailed(m_Device->CreateDescriptorHeap(&DescriptorHeapDesc, IID_PPV_ARGS(&m_DSVDescriptorHeap)));
	m_DSVDescriptorIncrementSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

void GameFramework::CreateRenderTargetView()
{
	D3D12_CPU_DESCRIPTOR_HANDLE CPUDescriptorHandle = m_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	for (UINT i = 0; i < FRAME_COUNT; i++)
	{
		ThrowIfFailed(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_SwapChainBuffer[i])));
		m_Device->CreateRenderTargetView(m_SwapChainBuffer[i].Get(), NULL, CPUDescriptorHandle);
		CPUDescriptorHandle.ptr += m_RTVDescriptorIncrementSize;
	}
}

void GameFramework::CreateDepthStencilView()
{
	D3D12_RESOURCE_DESC ResourceDesc;
	ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	ResourceDesc.Alignment = 0;
	ResourceDesc.Width = m_WndClientWidth;
	ResourceDesc.Height = m_WndClientHeight;
	ResourceDesc.DepthOrArraySize = 1;
	ResourceDesc.MipLevels = 1;
	ResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	ResourceDesc.SampleDesc.Count = (m_4XMSAA_Enabled) ? 4 : 1;
	ResourceDesc.SampleDesc.Quality = (m_4XMSAA_Enabled) ? (m_4XMSAA_Enabled - 1) : 0;
	ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	ResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES HeapProperties;
	::ZeroMemory(&HeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
	HeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	HeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	HeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	HeapProperties.CreationNodeMask = 1;
	HeapProperties.VisibleNodeMask = 1;

	D3D12_CLEAR_VALUE ClearValue;
	ClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	ClearValue.DepthStencil.Depth = 1.0f;
	ClearValue.DepthStencil.Stencil = 0;

	ThrowIfFailed(m_Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE,
		&ResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &ClearValue, IID_PPV_ARGS(&m_DepthStencilBuffer)));

	D3D12_DEPTH_STENCIL_VIEW_DESC DSVDesc;
	::ZeroMemory(&DSVDesc, sizeof(D3D12_DEPTH_STENCIL_VIEW_DESC));
	DSVDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DSVDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	DSVDesc.Flags = D3D12_DSV_FLAG_NONE;

	D3D12_CPU_DESCRIPTOR_HANDLE DSVCPUDescHandle = m_DSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_Device->CreateDepthStencilView(m_DepthStencilBuffer.Get(), &DSVDesc, DSVCPUDescHandle);
}

void GameFramework::PopulateCommandList()
{
	ThrowIfFailed(m_CommandAllocator->Reset());
	ThrowIfFailed(m_CommandList->Reset(m_CommandAllocator.Get(), NULL));

	D3D12_RESOURCE_BARRIER ResourceBarrier;
	::ZeroMemory(&ResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
	ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	ResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	ResourceBarrier.Transition.pResource = m_SwapChainBuffer[m_FrameIndex].Get();
	ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	ResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	m_CommandList->ResourceBarrier(1, &ResourceBarrier);	

	D3D12_CPU_DESCRIPTOR_HANDLE RTVDescHandle = m_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	RTVDescHandle.ptr += (m_FrameIndex * m_RTVDescriptorIncrementSize);

	D3D12_CPU_DESCRIPTOR_HANDLE DSVDescHandle = m_DSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_CommandList->OMSetRenderTargets(1, &RTVDescHandle, FALSE, &DSVDescHandle);


	float ClearColor[4] = { 0.94f, 0.8f, 0.64f, 1.0f };
	m_CommandList->ClearRenderTargetView(RTVDescHandle, ClearColor, 0, NULL);
	m_CommandList->ClearDepthStencilView(DSVDescHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);

	//Render here before closing the Command List and before transitioning the Resource State to present	
	m_Scenes.top()->Render(m_CommandList.Get(), m_Camera, m_Timer.Interpolation());

	ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	ResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_CommandList->ResourceBarrier(1, &ResourceBarrier);

	ThrowIfFailed(m_CommandList->Close());
}

void GameFramework::ToggleFullscreen()
{
	WaitForGPU();

	BOOL bFullScreenState = FALSE;
	m_SwapChain->GetFullscreenState(&bFullScreenState, NULL);
	m_SwapChain->SetFullscreenState(!bFullScreenState, NULL);

	DXGI_MODE_DESC dxgiTargetParameters;
	dxgiTargetParameters.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiTargetParameters.Width = m_WndClientWidth;
	dxgiTargetParameters.Height = m_WndClientHeight;
	dxgiTargetParameters.RefreshRate.Numerator = 60;
	dxgiTargetParameters.RefreshRate.Denominator = 1;
	dxgiTargetParameters.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	dxgiTargetParameters.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	m_SwapChain->ResizeTarget(&dxgiTargetParameters);

	for (int i = 0; i < FRAME_COUNT; i++)
		m_SwapChainBuffer[i].Reset();

	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	m_SwapChain->GetDesc(&dxgiSwapChainDesc);
	m_SwapChain->ResizeBuffers(FRAME_COUNT, m_WndClientWidth, m_WndClientHeight, dxgiSwapChainDesc.BufferDesc.Format, dxgiSwapChainDesc.Flags);

	m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();


	CreateRenderTargetView();
}

void GameFramework::GetWindowSize(int * Width, int * Height) const
{
	*Width = m_WndClientWidth;
	*Height = m_WndClientHeight;
}

void GameFramework::WaitForGPU()
{
	UINT64 FenceValue = ++m_FenceValues[m_FrameIndex];
	ThrowIfFailed(m_CommandQueue->Signal(m_Fence.Get(), FenceValue));
	if (m_Fence->GetCompletedValue() < FenceValue)
	{
		ThrowIfFailed(m_Fence->SetEventOnCompletion(FenceValue, m_FenceEvent));
		WaitForSingleObject(m_FenceEvent, INFINITE);
	}
}

void GameFramework::MoveToNextFrame()
{
	m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();

	UINT64 nFenceValue = ++m_FenceValues[m_FrameIndex];
	ThrowIfFailed(m_CommandQueue->Signal(m_Fence.Get(), nFenceValue));

	if (m_Fence->GetCompletedValue() < nFenceValue)
	{
		ThrowIfFailed(m_Fence->SetEventOnCompletion(nFenceValue, m_FenceEvent));
		WaitForSingleObject(m_FenceEvent, INFINITE);
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
	static Scene* pScene{ NULL };
	pScene = m_Scenes.size() ? m_Scenes.top() : NULL;

	switch (uMsg)
	{
	case WM_ACTIVATE:
	case WM_SIZE:
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_COMMAND:
		if(pScene) pScene->WndProc(hWnd, uMsg, wParam, lParam);
		break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = ::BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
	default:
		return(::DefWindowProc(hWnd, uMsg, wParam, lParam));
	}
	return 0;
}

HWND GameFramework::GetWindowHandle() const
{
	return m_hWnd;
}

Player * GameFramework::GetCurrentPlayer() const
{
	return m_Player;
}
