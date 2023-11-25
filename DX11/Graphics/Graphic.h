#pragma once
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <cassert>
#include <cmath>
#include <DirectXMath.h>
#include <comdef.h>
#include <wrl/client.h>

#include "Model.h"
#include "ConstBufferType.h"
#include "Rasterizer.h"
#include "Shaders.h"

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
	void BeginFrame(const HWND HWnd);
	void Draw(Model model, const float Angle, float x, float y, float z);
	void Present();
	void ClearView();
	void CleanD3D() const;
	Model bear;
	Model Cube;
	Model D20;
	Model D12;
	Model D10;
	Model D4;
	Model D8;

private:
	UINT offset = 0;

	// Global declarations for Direct3D
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapchain;            // Pointer to the swap chain interface
	Microsoft::WRL::ComPtr<ID3D11Device> D3D_device;                     // Pointer to our Direct3D device interface
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> D3D_device_context;           // Pointer to our Direct3D device context
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> backbuffer;   // Pointer to our Render Target View(AKA our back buffer)

	VertexShader vertexShader;
	PixelShader pixelShader;
	ConstantBuffer<ConstBuffer> constantBuffer;
	Rasterizer rasterizer;

	ID3D11DepthStencilState* pDSState = nullptr;
	ID3D11DepthStencilView* pDSV = nullptr;
	ID3D11SamplerState* pSS = nullptr;
};
