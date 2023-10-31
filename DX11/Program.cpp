// include the basic windows header files and the Direct3D header files
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <cassert>

using RGBA = float[4]; //Allows for us to create an RGBA vlaue
struct VERTEX { FLOAT X, Y, Z; RGBA Color; };    // a struct to define a vertex
static UINT numVerts;
static UINT stride;
static UINT offset;

// include the Direct3D Library file
#pragma comment( lib, "user32" )          // link against the win32 library
#pragma comment( lib, "d3d11.lib" )       // direct3D library
#pragma comment( lib, "dxgi.lib" )        // directx graphics interface
#pragma comment( lib, "d3dcompiler.lib" ) // shader compiler

// Global declarations for Direct3D
static IDXGISwapChain* swapchain;             // Pointer to the swap chain interface
static ID3D11Device* D3D_device;                     // Pointer to our Direct3D device interface
static ID3D11DeviceContext* D3D_device_context;           // Pointer to our Direct3D device context
static ID3D11RenderTargetView* backbuffer;    // Pointer to our Render Target View(AKA our back buffer)
static ID3D11Buffer* pVBuffer;     //Vertex Buffer pointer
static ID3D11VertexShader* pVS;    // Vertex shader pointer
static ID3D11PixelShader* pPS;     // Pixel shader pointer
static ID3D11InputLayout* pLayout;    // Input Layout pointer

static void CleanD3D();

// WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// define the screen resolution
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600



// Main entry point for the Windows program
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
#pragma region WindowStuff



    HWND hWnd;
    WNDCLASSEX wc;

    ZeroMemory(&wc, sizeof(WNDCLASSEX)); //Initialize wc by clearing our any existing used memory

    wc.cbSize = sizeof(WNDCLASSEX); //Specifies the Size of the Structure
    wc.style = CS_HREDRAW | CS_VREDRAW; //Style of window class we want
    wc.lpfnWndProc = WindowProc; //Defines what method will run
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    //wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszClassName = "WindowClass";

    RegisterClassEx(&wc);

    RECT wr = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

    hWnd = CreateWindowEx(NULL,
        "WindowClass",
        "Our First Direct3D Program",
        WS_OVERLAPPEDWINDOW,
        300,
        300,
        wr.right - wr.left,
        wr.bottom - wr.top,
        NULL,
        NULL,
        hInstance,
        NULL);

    ShowWindow(hWnd, nCmdShow);
#pragma endregion

#pragma region D3D



    OutputDebugStringA("Began Initialization of Direct 3D");

#ifdef DEBUG_BUILD
    // Set up debug layer to break on D3D11 errors
    ID3D11Debug* d3dDebug = nullptr;
    d3d11Device->QueryInterface(__uuidof(ID3D11Debug), (void**)&d3dDebug);
    if (d3dDebug)
    {
        ID3D11InfoQueue* d3dInfoQueue = nullptr;
        if (SUCCEEDED(d3dDebug->QueryInterface(__uuidof(ID3D11InfoQueue), (void**)&d3dInfoQueue)))
        {
            d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
            d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
            d3dInfoQueue->Release();
        }
        d3dDebug->Release();
    }
#endif

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Create Device
    ///////////////////////////////////////////////////////////////////////////////////////////
    HRESULT hr = D3D11CreateDevice
    (
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &D3D_device,
        nullptr,
        &D3D_device_context
    );
    if (FAILED(hr))
    {
        MessageBox(0, "D3D11CreateDevice() failed", "Fatal Errror", MB_OK);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Create Swapchain
    ///////////////////////////////////////////////////////////////////////////////////////////

    IDXGIFactory* dxgiFactory;
    {
        IDXGIDevice1* dxgiDevice;
        HRESULT hResult = D3D_device->QueryInterface(__uuidof(IDXGIDevice1), (void**)&dxgiDevice);
        assert(SUCCEEDED(hResult));

        IDXGIAdapter* dxgiAdapter;
        hResult = dxgiDevice->GetAdapter(&dxgiAdapter);
        assert(SUCCEEDED(hResult));
        dxgiDevice->Release();

        DXGI_ADAPTER_DESC adapterDesc;
        dxgiAdapter->GetDesc(&adapterDesc);

        hResult = dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory);
        assert(SUCCEEDED(hResult));
        dxgiAdapter->Release();
    }

    // Create a struct about the swap chain
    DXGI_SWAP_CHAIN_DESC scd;

    // clear out the struct for use
    ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

    // fill the swap chain description struct
    scd.BufferCount = 1;                                                            // We currently only want one back buffer
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;                             // use 32-bit color
    scd.BufferDesc.Width = SCREEN_WIDTH;                                            // set back buffer witdth
    scd.BufferDesc.Height = SCREEN_HEIGHT;                                          // set back buffer height
    scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;                         // set scaling mode to unspecified
    scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;         // set scanline ordering mode to unspecified
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;                              // how swap chain is to be used
    scd.OutputWindow = hWnd;                                                        // the window to be used
    scd.SampleDesc.Count = 1;                                                       // how many multisamples
    scd.SampleDesc.Quality = 0;                                                     // quality of anti-aliasing
    scd.Windowed = TRUE;                                                            // windowed/full-screen mode
    scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    // Create SwapChain using the information in the scd struct
    #pragma warning(suppress : 6387)
    hr = dxgiFactory->CreateSwapChain
    (
        D3D_device,
        &scd,
        &swapchain
    );
    if (FAILED(hr))
    {
        MessageBox(0, "DxgiCreateSwapchain() failed", "Fatal Errror", MB_OK);
    }

    OutputDebugStringA("Created Device and SwapChain");

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Create Back Buffer
    ///////////////////////////////////////////////////////////////////////////////////////////

    // Get the address of the back buffer
    ID3D11Texture2D* pBackBuffer;
    swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

    // use the back buffer address to create the render target
    #pragma warning(suppress : 6387)
    hr = D3D_device->CreateRenderTargetView(pBackBuffer, NULL, &backbuffer);
    assert(SUCCEEDED(hr));
    pBackBuffer->Release();

    OutputDebugStringA("Created render target view done");

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Create and set Pixel and Vertex Shaders
    ///////////////////////////////////////////////////////////////////////////////////////////

    // Compile the vertex and Pixel Shaders
    ID3DBlob* VS, * PS, * error_blob;
    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
    D3DCompileFromFile(L"VertexShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0", flags, 0, &VS, &error_blob);
    D3DCompileFromFile(L"PixelShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_5_0", flags, 0, &PS, &error_blob);

    OutputDebugStringA("Shader Compile from file complete");

    // Create Shader Objects
    hr = D3D_device->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &pVS);
    assert(SUCCEEDED(hr));
    OutputDebugStringA("Vertex Shader Created");

    hr = D3D_device->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &pPS);
    assert(SUCCEEDED(hr));
    OutputDebugStringA("Pixel Shader Created");


    ///////////////////////////////////////////////////////////////////////////////////////////
    // Create Input Layout
    ///////////////////////////////////////////////////////////////////////////////////////////

    //Create the input layout object - 
    D3D11_INPUT_ELEMENT_DESC ied[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    OutputDebugStringA("Input element created");

    hr = D3D_device->CreateInputLayout(ied, ARRAYSIZE(ied), VS->GetBufferPointer(), VS->GetBufferSize(), &pLayout);
    assert(SUCCEEDED(hr));

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Create Vertex Structure
    ///////////////////////////////////////////////////////////////////////////////////////////

    // create a triangle using the VERTEX struct
    VERTEX OurVertices[] =
    {
        {-0.5f, 0.5f, 0.0f,  1.0f, 0.0f, 0.0f, 1.0f},
        {0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f},
        {-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f},

        {-0.5f, 0.5f, 0.0f,  1.0f, 0.0f, 0.0f, 1.0f},
        {0.5f, 0.5f, 0.0f,  1.0f, 0.0f, 0.0f, 1.0f},
        {0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f},
    };
    UINT stride = sizeof(VERTEX);
    numVerts = sizeof(OurVertices) / stride;
    UINT offset = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Create Vertex Buffer
    ///////////////////////////////////////////////////////////////////////////////////////////

    // create the vertex buffer
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));

    bd.Usage = D3D11_USAGE_DYNAMIC;                // Write access by CPU and Read-Only GPU Access
    bd.ByteWidth = sizeof(OurVertices);             // Size is the VERTEX struct * 3
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // Use as Vertex Buffer
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // Allow CPU to write in buffer

    D3D_device->CreateBuffer(&bd, NULL, &pVBuffer);       // Create the buffer

    D3D11_MAPPED_SUBRESOURCE ms;
#pragma warning(suppress : 6387)
    hr = D3D_device_context->Map(pVBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);   // map the buffer
    assert(SUCCEEDED(hr));
    memcpy(ms.pData, OurVertices, sizeof(OurVertices));                // copy the data
#pragma warning(suppress : 6387)
    D3D_device_context->Unmap(pVBuffer, NULL);                                     // unmap the buffer

#pragma endregion
    // enter the main loop:

    MSG msg;

    while (TRUE)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT)

                break;
        }
        
        // clear the back buffer to a deep blue
        D3D_device_context->ClearRenderTargetView(backbuffer, RGBA(0.1f, 0.1f, 1.0f, 1.0f));

        // Set the viewport
        RECT winRect;
        GetClientRect(hWnd, &winRect);
        D3D11_VIEWPORT viewport = { 0.0f, 0.0f, (FLOAT)(winRect.right - winRect.left), (FLOAT)(winRect.bottom - winRect.top), 0.0f, 1.0f };
        D3D_device_context->RSSetViewports(1, &viewport);

        D3D_device_context->OMSetRenderTargets(1, &backbuffer, NULL);

        //Input Assembly Stage
        D3D_device_context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        D3D_device_context->IASetInputLayout(pLayout);
        D3D_device_context->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);

        //Vertex and Pixel shader stage
        D3D_device_context->VSSetShader(pVS, 0, 0);
        D3D_device_context->PSSetShader(pPS, 0, 0);

        // draw the vertex buffer to the back buffer
        D3D_device_context->Draw(numVerts, 0);

        // switch the back buffer and the front buffer
        swapchain->Present(1, 0);
    }

    // clean up DirectX and COM
    CleanD3D();
    OutputDebugStringA("Cleanup D3d");

    return (int)msg.wParam;
}


// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        return 0;
    } break;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

// this is the function that cleans up Direct3D and COM
void CleanD3D(void)
{
    swapchain->SetFullscreenState(FALSE, NULL);    // switch to windowed mode

    // close and release all existing COM objects
    pVS->Release();
    pPS->Release();
    pLayout->Release();
    pVBuffer->Release();
    swapchain->Release();
    backbuffer->Release();
    D3D_device->Release();
    D3D_device_context->Release();
}


