// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>


// TODO: reference additional headers your program requires here


#include "PubMacroDef.h"
#include "GlobalConfig.h"
#include <d3d11.h>
#include <dxgi.h>

#define DIRECTX_2010
#ifdef DIRECTX_2010
// 禁止按16字节对齐
#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <xnamath.h>
struct DllExport _XMFLOAT4;
struct DllExport _XMMATRIX;
#include <d3dx11Async.h>
//// 链接
#pragma comment(lib, "d3d11.lib")   
#pragma comment(lib, "dxgi.lib")	 
#pragma comment(lib, "d3dx11.lib")   
//$(DirectXIncludePath)
//$(DirectXLibPath)
//d3dx10.lib
//dxerr.libd3d11.lib
//d3dx11.lib
//d3dx10.lib
//dxerr.lib
#else
//在Win8及以上平台可用
#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <directxmath.h>
#include <d3dcompiler.h>
using namespace DirectX;
struct DllExport DirectX::XMMATRIX;
struct DllExport DirectX::XMFLOAT4;
//// 链接
#pragma comment(lib, "d3d11.lib")		// contains all the Direct3D functionality
#pragma comment(lib, "dxgi.lib")	 //contains tools to interface with the hardware
#pragma comment(lib, "d3dcompiler.lib")  //contains functionality for compiling shaders
#endif

#define MEMCPY_MATRIX(A,B) A = B



#include <stdio.h>
#include <fstream>
#include <map>
using std::ofstream;
using std::ifstream;
using std::map;


// 为了消除warning
//#pragma warning( disable: 4251 )
//#include <d3dcommon.h>
