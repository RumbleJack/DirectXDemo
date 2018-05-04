#pragma once 
//#define DIRECTX_2010

#include "PubMacroDef.h"
#include "GlobalConfig.h"
#include <d3d11.h>
#include <dxgi.h>

#ifdef DIRECTX_2010
// ��ֹ��16�ֽڶ���
#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <xnamath.h>
struct DllExport _XMFLOAT4;
struct DllExport _XMMATRIX;
#include <d3dx11Async.h>
//// ����
	#pragma comment(lib, "d3d11.lib")   
	#pragma comment(lib, "dxgi.lib")	 
	#pragma comment(lib, "d3dx11.lib")   
#define MEMCPY_MATRIX(A,B) memcpy(reinterpret_cast<float*>(A.m), reinterpret_cast<float*>(B.m), 16 * sizeof(float))
#else
//��Win8������ƽ̨����
#include <directxmath.h>
#include <d3dcompiler.h>
using namespace DirectX;
//// ����
	#pragma comment(lib, "d3d11.lib")		// contains all the Direct3D functionality
	
	#pragma comment(lib, "dxgi.lib")	 //contains tools to interface with the hardware
	#pragma comment(lib, "d3dcompiler.lib")  //contains functionality for compiling shaders 
	//#pragma comment(lib, "d3dx11.lib")   	
	//#pragma comment(lib, "d3dx10.lib")   
	//#pragma comment(lib, "dxerr.lib")  
#define MEMCPY_MATRIX(A,B) A = B
#endif






