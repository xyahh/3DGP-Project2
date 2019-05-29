#pragma once

#include <Windows.h>
#include <string>
#include <wrl.h>
#include <shellapi.h>


#include <d3d12.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>

#include "DXHelper.h"

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

/* to use in header files */
#define _3DGP_BEGIN_ namespace _3DGP_ {
#define _3DGP_END_	}

/* to use in CPP files */
#define _3DGP_USE_	using namespace _3DGP_;

/* Namespace macros */
#define DX_USE using namespace DirectX;
#define DX	DirectX::
#define DXV DirectX::PackedVector::
#define WRL Microsoft::WRL::

#define _WITH_SWAPCHAIN_FULLSCREEN_STATE

#include "DXHelper2.h"


