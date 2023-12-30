#pragma once
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <cassert>
#include <cmath>
#include <DirectXMath.h>
#include <comdef.h>
#include <wrl/client.h>
#include <list>

#include "Model.h"
#include "ConstBufferType.h"
#include "Rasterizer.h"
#include "Shaders.h"
#include "..\\DiceRollerHelper.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"


using namespace DirectX;
using namespace std;
using vec3 = float[3];

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
	void Draw(Model model, float x, float y, float z, float xScale, float yScale, float zScale);
	void Present();
	void ClearView(rgba color);
	void CleanD3D() const;
	Model D6;
	Model D20;
	Model D12;
	Model D10;
	Model D4;
	Model D8;
	DiceRollerHelper rollerHelp;

	rgba backColour;

	std::vector<Model> models;
private:
	float faceNum;
	UINT offset = 0;
	vec3 rotOffset;
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
