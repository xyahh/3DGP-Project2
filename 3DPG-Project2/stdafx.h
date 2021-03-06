#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>

#include <algorithm>

#include <wrl.h>
#include <list>

#include <d3d12.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#define KEY_PRESSED(pKeyBuffer, VirtualKey) if(pKeyBuffer[VirtualKey] & 0xF0)

/* to use in header files */
#define _3DGP_BEGIN_ namespace _3DGP_ {
#define _3DGP_END_	}

/* to use in CPP files */
#define _3DGP_USE_	using namespace _3DGP_;

/* Other namespace macros */
#define DX_USE using namespace DirectX;
#define DX	DirectX::
#define DX_BEGIN namespace DirectX {
#define DX_END }
#define STD std::
#define MWRL Microsoft::WRL::

/*
-Explicit Child: It must be a child. Will NOT compile if T == Base.
-Implicit Child: Can be a child or the base itself. Will compile if T == Base.
*/
#define _3DGP_EXPLICIT_CHILD_OF_TEMPLATE_(T, Base) template<class T, typename = std::enable_if<std::is_base_of<Base, T>::value && !std::is_same<Base, T>::value>::type>
#define _3DGP_IMPLICIT_CHILD_OF_TEMPLATE_(T, Base) template<class T, typename = std::enable_if<std::is_base_of<Base, T>::value>::type>
#define _3DGP_IMPLICIT_CHILD_OF_TEMPLATE_EXT_(T, Base, Ext) template<class T, typename = std::enable_if<std::is_base_of<Base, T>::value>::type, Ext>

#include "DXMath.h"
#include "DXInclude.h"
#include "Timer.h"
#include "FunctionLibrary.h"