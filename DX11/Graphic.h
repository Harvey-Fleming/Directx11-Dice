#pragma once
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <cassert>
#include <cmath>
#include <DirectXMath.h>
#include <comdef.h>

#include "Vertex.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include "ConstBufferType.h"

using namespace DirectX;

// include the Direct3D Library file
#pragma comment( lib, "user32" )          // link against the win32 library
#pragma comment( lib, "d3d11.lib" )       // direct3D library
#pragma comment( lib, "dxgi.lib" )        // directx graphics interface
#pragma comment( lib, "d3dcompiler.lib" ) // shader compiler


class Graphic
{
public:
	Graphic(HWND HWnd);
	Graphic(const Graphic&) = delete;
	Graphic& operator=(const Graphic&) = delete;
	~Graphic() = default;
	void DrawTestTriangle(HWND HWnd, float Angle, float x, float z);
	void Present();
	void ClearView();
	void CleanD3D() const;

private:
	UINT offset = 0;

	// Global declarations for Direct3D
	IDXGISwapChain* swapchain;             // Pointer to the swap chain interface
	ID3D11Device* D3D_device;                     // Pointer to our Direct3D device interface
	ID3D11DeviceContext* D3D_device_context;           // Pointer to our Direct3D device context
	ID3D11RenderTargetView* backbuffer = nullptr;    // Pointer to our Render Target View(AKA our back buffer)

	VertexBuffer<Vertex> vertexBuffer;
	IndexBuffer indexBuffer;

	ID3D11Buffer* pIBuffer = nullptr;     //Index Buffer pointer
	ConstantBuffer<ConstBuffer> constantBuffer;
	ID3D11VertexShader* pVS = nullptr;    // Vertex shader pointer
	ID3D11PixelShader* pPS = nullptr;     // Pixel shader pointer
	ID3D11InputLayout* pLayout = nullptr;    // Input Layout pointer
	ID3D11DepthStencilState* pDSState = nullptr;
	ID3D11DepthStencilView* pDSV = nullptr;
	ID3DBlob* VS, * PS, * error_blob;
};
