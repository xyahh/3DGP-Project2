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

	m_Factory = NULL;
	m_SwapChain = NULL;
	m_Device = NULL;

	for (int i = 0; i < FRAME_COUNT; i++) m_SwapChainBuffer[i] = NULL;
	m_FrameIndex = 0;

	m_CommandAllocator = NULL;
	m_CommandQueue = NULL;
	m_CommandList = NULL;

	m_RTVDescriptorHeap = NULL;
	m_DSVDescriptorHeap = NULL;

	m_DepthStencilBuffer = NULL;

	m_RTVDescriptorIncrementSize = 0;
	m_DSVDescriptorIncrementSize = 0;

	m_Fence = NULL;
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

	if (m_DepthStencilBuffer) m_DepthStencilBuffer->Release();
	if (m_DSVDescriptorHeap) m_DSVDescriptorHeap->Release();

	for (int i = 0; i < FRAME_COUNT; i++) if (m_SwapChainBuffer[i]) m_SwapChainBuffer[i]->Release();
	if (m_RTVDescriptorHeap) m_RTVDescriptorHeap->Release();

	if (m_CommandAllocator) m_CommandAllocator->Release();
	if (m_CommandQueue)		m_CommandQueue->Release();
	if (m_CommandList)		m_CommandList->Release();
	if (m_Fence)			m_Fence->Release();

	m_SwapChain->SetFullscreenState(FALSE, NULL);
	if (m_SwapChain)	m_SwapChain->Release();
	if (m_Device)		m_Device->Release();
	if (m_Factory)		m_Factory->Release();

	if (m_Camera) delete m_Camera;
}

void GameFramework::InitWindow(const std::string& title, int width, int height)
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

	m_HWND = CreateWindow(lpszClass, m_WndTitle.c_str(), dwStyle, Center.x, Center.y,
		WindowRect.right - WindowRect.left,
		WindowRect.bottom - WindowRect.top,
		NULL, NULL, m_hInstance, NULL);

	ShowWindow(m_HWND, SW_SHOW);
	UpdateWindow(m_HWND);
}

void GameFramework::InitCamera()
{
	if (m_Camera) delete m_Camera;
	m_Camera = new Camera;
	m_Camera->SetViewport(0, 0, m_WndClientWidth, m_WndClientHeight);
	m_Camera->SetScissorRect(0, 0, (LONG)m_WndClientWidth, (LONG)m_WndClientHeight);
	m_Camera->GenerateProjMatrix(1.f, 1500.f, float(m_WndClientWidth) / float(m_WndClientHeight), 90.f);
	m_Camera->GenerateViewMatrix(XMFLOAT3(0.f, 15.f, -500.f), XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(0.f, 1.f, 0.f));
}

void GameFramework::UpdateClientRect()
{
	RECT ClientRect;
	::GetClientRect(m_HWND, &ClientRect);
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
			m_Scenes.top()->Update(m_Timer.GetDeltaTime());

		//Render is called in this function
		PopulateCommandList();

		ID3D12CommandList* CommandLists[] = { m_CommandList };

		m_CommandQueue->ExecuteCommandLists(_countof(CommandLists), CommandLists);

		WaitForGPU();

#ifdef _WITH_SYNCH_VERTICAL_RETRACE
		ThrowIfFailed(m_SwapChain->Present(1, 0));
#else
		ThrowIfFailed(m_SwapChain->Present(0, 0));
#endif
		MoveToNextFrame();
		::SetWindowText(m_HWND, (m_WndTitle + " (FPS: " + m_Timer.GetFPS() + ")").c_str());
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
		ID3D12Debug* pDebugController;
		ThrowIfFailed(D3D12GetDebugInterface(__uuidof(ID3D12Debug), (void **)&pDebugController));
		if (pDebugController)
		{
			pDebugController->EnableDebugLayer();
			pDebugController->Release();
		}
#endif
	}
}

void GameFramework::CreateD3Device()
{
	ThrowIfFailed(CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void **)&m_Factory));

	IDXGIAdapter1 *pAdapter = NULL;

	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != m_Factory->EnumAdapters1(i, &pAdapter); ++i)
	{
		DXGI_ADAPTER_DESC1 AdapterDesc;
		pAdapter->GetDesc1(&AdapterDesc);
		if (AdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;
		if (SUCCEEDED(D3D12CreateDevice(pAdapter, D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), (void **)&m_Device))) break;
	}

	if (!pAdapter)
	{
		m_Factory->EnumWarpAdapter(_uuidof(IDXGIFactory4), (void **)&pAdapter);
		ThrowIfFailed(D3D12CreateDevice(pAdapter, D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), (void **)&m_Device));
	}

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS MSAAQualityLevels;
	MSAAQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	MSAAQualityLevels.SampleCount = 4;
	MSAAQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	MSAAQualityLevels.NumQualityLevels = 0;
	ThrowIfFailed(m_Device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &MSAAQualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS)));
	m_4XMSAA_QualityLevels = MSAAQualityLevels.NumQualityLevels;
	m_4XMSAA_Enabled = (m_4XMSAA_QualityLevels > 1) ? true : false;

	ThrowIfFailed(m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void **)&m_Fence));
	for (UINT i = 0; i < FRAME_COUNT; ++i) m_FenceValues[i] = 0;

	m_FenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	if (pAdapter) pAdapter->Release();
}

void GameFramework::CreateCommandInterfaces()
{
	D3D12_COMMAND_QUEUE_DESC CommandQueueDesc;
	::ZeroMemory(&CommandQueueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));
	CommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	CommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	ThrowIfFailed(m_Device->CreateCommandQueue(&CommandQueueDesc, _uuidof(ID3D12CommandQueue), (void **)&m_CommandQueue));

	ThrowIfFailed(m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void **)&m_CommandAllocator));

	ThrowIfFailed(m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAllocator, NULL, __uuidof(ID3D12GraphicsCommandList), (void **)&m_CommandList));
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
	SwapChainDesc.OutputWindow = m_HWND;
	SwapChainDesc.SampleDesc.Count = (m_4XMSAA_Enabled) ? 4 : 1;
	SwapChainDesc.SampleDesc.Quality = (m_4XMSAA_QualityLevels) ? (m_4XMSAA_QualityLevels - 1) : 0;
	SwapChainDesc.Windowed = TRUE;
	SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	ThrowIfFailed(m_Factory->CreateSwapChain(m_CommandQueue, &SwapChainDesc, (IDXGISwapChain **)&m_SwapChain));

	m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();

	ThrowIfFailed(m_Factory->MakeWindowAssociation(m_HWND, DXGI_MWA_NO_ALT_ENTER));

	
}

void GameFramework::CreateRTVDSVDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeapDesc;
	::ZeroMemory(&DescriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	DescriptorHeapDesc.NumDescriptors = FRAME_COUNT;
	DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	DescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	DescriptorHeapDesc.NodeMask = 0;

	ThrowIfFailed(m_Device->CreateDescriptorHeap(&DescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void **)&m_RTVDescriptorHeap));
	m_RTVDescriptorIncrementSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	DescriptorHeapDesc.NumDescriptors = 1;
	DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

	ThrowIfFailed(m_Device->CreateDescriptorHeap(&DescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void **)&m_DSVDescriptorHeap));
	m_DSVDescriptorIncrementSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

void GameFramework::CreateRenderTargetView()
{
	D3D12_CPU_DESCRIPTOR_HANDLE CPUDescriptorHandle = m_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	for (UINT i = 0; i < FRAME_COUNT; i++)
	{
		ThrowIfFailed(m_SwapChain->GetBuffer(i, __uuidof(ID3D12Resource), (void **)&m_SwapChainBuffer[i]));
		m_Device->CreateRenderTargetView(m_SwapChainBuffer[i], NULL, CPUDescriptorHandle);
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
		&ResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &ClearValue, __uuidof(ID3D12Resource), (void **)&m_DepthStencilBuffer));

	D3D12_DEPTH_STENCIL_VIEW_DESC DSVDesc;
	::ZeroMemory(&DSVDesc, sizeof(D3D12_DEPTH_STENCIL_VIEW_DESC));
	DSVDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DSVDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	DSVDesc.Flags = D3D12_DSV_FLAG_NONE;

	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle = m_DSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_Device->CreateDepthStencilView(m_DepthStencilBuffer, &DSVDesc, d3dDsvCPUDescriptorHandle);
}

void GameFramework::PopulateCommandList()
{
	ThrowIfFailed(m_CommandAllocator->Reset());
	ThrowIfFailed(m_CommandList->Reset(m_CommandAllocator, NULL));

	D3D12_RESOURCE_BARRIER ResourceBarrier;
	::ZeroMemory(&ResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
	ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	ResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	ResourceBarrier.Transition.pResource = m_SwapChainBuffer[m_FrameIndex];
	ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	ResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	m_CommandList->ResourceBarrier(1, &ResourceBarrier);	

	D3D12_CPU_DESCRIPTOR_HANDLE RTVDescHandle = m_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	RTVDescHandle.ptr += (m_FrameIndex * m_RTVDescriptorIncrementSize);

	D3D12_CPU_DESCRIPTOR_HANDLE DSVDescHandle = m_DSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_CommandList->OMSetRenderTargets(1, &RTVDescHandle, FALSE, &DSVDescHandle);

	float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };
	m_CommandList->ClearRenderTargetView(RTVDescHandle, ClearColor, 0, NULL);
	m_CommandList->ClearDepthStencilView(DSVDescHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);

	//Render here before closing the Command List and before transitioning the Resource State to present	
	m_Scenes.top()->Render(m_CommandList, m_Camera, m_Timer.Interpolation());

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

	for (int i = 0; i < FRAME_COUNT; i++) if (m_SwapChainBuffer[i]) m_SwapChainBuffer[i]->Release();

	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	m_SwapChain->GetDesc(&dxgiSwapChainDesc);
	m_SwapChain->ResizeBuffers(FRAME_COUNT, m_WndClientWidth, m_WndClientHeight, dxgiSwapChainDesc.BufferDesc.Format, dxgiSwapChainDesc.Flags);

	m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();

	CreateRenderTargetView();

}

void GameFramework::WaitForGPU()
{
	UINT64 FenceValue = ++m_FenceValues[m_FrameIndex];
	ThrowIfFailed(m_CommandQueue->Signal(m_Fence, FenceValue));
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
	ThrowIfFailed(m_CommandQueue->Signal(m_Fence, nFenceValue));

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
	switch (uMsg)
	{
	case WM_ACTIVATE:
	case WM_SIZE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		break;
	case WM_KEYDOWN:
		break;
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_RETURN:
			ToggleFullscreen();
			break;
		}
		break;
	case WM_COMMAND:
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
