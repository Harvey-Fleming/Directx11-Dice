#pragma once
#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <cassert>
#include <cmath>

// include the Direct3D Library file
#pragma comment( lib, "user32" )          // link against the win32 library
#pragma comment( lib, "d3d11.lib" )       // direct3D library
#pragma comment( lib, "dxgi.lib" )        // directx graphics interface
#pragma comment( lib, "d3dcompiler.lib" ) // shader compiler


class Graphic
{
public:
	Graphic(HWND hWnd);
	Graphic(const Graphic&) = delete;
	Graphic& operator=(const Graphic&) = delete;
	~Graphic() = default;
	void DrawTestTriangle(HWND hWnd, float angle);
	void CleanD3D();
private:
	using RGBA = float[4]; //Allows for us to create an RGBA vlaue
	struct VERTEX { FLOAT X, Y, Z; RGBA Color; };    // a struct to define a vertex
	UINT stride = 0;
	UINT numVerts = 0;
	UINT offset = 0;

	// Global declarations for Direct3D
	IDXGISwapChain* swapchain;             // Pointer to the swap chain interface
	ID3D11Device* D3D_device;                     // Pointer to our Direct3D device interface
	ID3D11DeviceContext* D3D_device_context;           // Pointer to our Direct3D device context
	ID3D11RenderTargetView* backbuffer;    // Pointer to our Render Target View(AKA our back buffer)
	ID3D11Buffer* pVBuffer;     //Vertex Buffer pointer
	ID3D11Buffer* pIBuffer;     //Vertex Buffer pointer
	ID3D11Buffer* pCBuffer;
	ID3D11VertexShader* pVS;    // Vertex shader pointer
	ID3D11PixelShader* pPS;     // Pixel shader pointer
	ID3D11InputLayout* pLayout;    // Input Layout pointer
};
