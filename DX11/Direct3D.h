#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>

using RGBA = float[4]; //Allows for us to create an RGBA vlaue
struct VERTEX { FLOAT X, Y, Z; RGBA Color; };    // a struct to define a vertex

// include the Direct3D Library file
#pragma comment (lib, "d3d11.lib")

// Global declarations for Direct3D
static IDXGISwapChain* swapchain;             // Pointer to the swap chain interface
static ID3D11Device* dev;                     // Pointer to our Direct3D device interface
static ID3D11DeviceContext* devcon;           // Pointer to our Direct3D device context
static ID3D11RenderTargetView* backbuffer;    // Pointer to our Render Target View(AKA our back buffer)
static ID3D11Buffer* pVBuffer;     //Vertex Buffer pointer
static ID3D11VertexShader* pVS;    // Vertex shader pointer
static ID3D11PixelShader* pPS;     // Pixel shader pointer
static ID3D11InputLayout* pLayout;    // Input Layout pointer

static void InitD3D(HWND hWnd, UINT SCREEN_WIDTH, UINT SCREEN_HEIGHT);

static void CleanD3D(void);

static void RenderFrame(void);

static void InitPipeline(void);

static void InitGraphics(void);


