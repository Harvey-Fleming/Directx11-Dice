#include "Graphic.h"

Graphic::Graphic(const HWND HWnd)
{
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
        D3D_device.GetAddressOf(),
        nullptr,
        D3D_device_context.GetAddressOf()
    );
    if (FAILED(hr))
    {
        MessageBox(nullptr, "D3D11CreateDevice() failed", "Fatal Errror", MB_OK);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Create Swapchain
    ///////////////////////////////////////////////////////////////////////////////////////////

    IDXGIFactory* dxgiFactory;
    {
        IDXGIDevice1* dxgiDevice;
        HRESULT hResult = D3D_device->QueryInterface(__uuidof(IDXGIDevice1), reinterpret_cast<void**>(&dxgiDevice));
        assert(SUCCEEDED(hResult));

        IDXGIAdapter* dxgiAdapter;
        hResult = dxgiDevice->GetAdapter(&dxgiAdapter);
        assert(SUCCEEDED(hResult));
        dxgiDevice->Release();

        DXGI_ADAPTER_DESC adapterDesc;
        dxgiAdapter->GetDesc(&adapterDesc);

        hResult = dxgiAdapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&dxgiFactory));
        assert(SUCCEEDED(hResult));
        dxgiAdapter->Release();
    }

    // Create a struct about the swap chain
    DXGI_SWAP_CHAIN_DESC scd;

    // clear out the struct for use
    ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

    // fill the swap chain description struct
    scd.BufferCount = 1;                                                        // Set 1 back buffer
    scd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;                         // use 32-bit color
    scd.BufferDesc.Width = 0;                                                   // set back buffer width
    scd.BufferDesc.Height = 0;                                                  // set back buffer height
    scd.BufferDesc.RefreshRate.Numerator = 0;
    scd.BufferDesc.RefreshRate.Denominator = 0;
    scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;                     // set scaling mode to unspecified
    scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;     // set scanline ordering mode to unspecified
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;                          // how swap chain is to be used
    scd.OutputWindow = HWnd;                                                    // the window to be used
    scd.SampleDesc.Count = 1;                                                   // how many multi-samples
    scd.SampleDesc.Quality = 0;                                                 // quality of anti-aliasing
    scd.Windowed = TRUE;                                                        // windowed/full-screen mode
    scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    // Create SwapChain using the information in the scd struct
#pragma warning(suppress : 6387)
    hr = dxgiFactory->CreateSwapChain
    (
        D3D_device.Get(),
        &scd,
        &swapchain
    );
    if (FAILED(hr))
    {
        MessageBox(nullptr, "DxgiCreateSwapchain() failed", "Fatal Error", MB_OK);
    }

    OutputDebugStringA("Created Device and SwapChain");

    // Set the viewport
    RECT winRect;
    GetClientRect(HWnd, &winRect);
    const D3D11_VIEWPORT viewport = { 0.0f, 0.0f, static_cast<FLOAT>(winRect.right - winRect.left), static_cast<FLOAT>(winRect.bottom - winRect.top), 0.0f, 1.0f };
    D3D_device_context->RSSetViewports(1, &viewport);

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Create Back Buffer
    ///////////////////////////////////////////////////////////////////////////////////////////

    // Get the address of the back buffer
    ID3D11Texture2D* pBackBuffer;
    hr = swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&pBackBuffer));
    assert(SUCCEEDED(hr));

    // use the back buffer address to create the render target
    #pragma warning(suppress : 6387)
    hr = D3D_device->CreateRenderTargetView(pBackBuffer, nullptr, &backbuffer);
    assert(SUCCEEDED(hr));
    pBackBuffer->Release();

    OutputDebugStringA("Created render target view done");

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Create and set Pixel and Vertex Shaders
    ///////////////////////////////////////////////////////////////////////////////////////////

// Compile the vertex and Pixel Shaders
    constexpr UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
    hr = D3DCompileFromFile(L"VertexShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0", flags, 0, VS.GetAddressOf(), nullptr);
    assert(SUCCEEDED(hr));
    hr = D3DCompileFromFile(L"PixelShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_5_0", flags, 0, PS.GetAddressOf(), nullptr);
    assert(SUCCEEDED(hr));

    // Create Shader Objects
    hr = D3D_device->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &pVS);
    assert(SUCCEEDED(hr));
    OutputDebugStringA("Vertex Shader Created");

    hr = D3D_device->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &pPS);
    assert(SUCCEEDED(hr));
    OutputDebugStringA("Pixel Shader Created");

    OutputDebugStringA("Shader Compile from file complete");

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Create Depth stencil Buffer
    ///////////////////////////////////////////////////////////////////////////////////////////
    D3D11_DEPTH_STENCIL_DESC dsd = {};
    dsd.DepthEnable = TRUE;
    dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsd.DepthFunc = D3D11_COMPARISON_LESS;
    hr = D3D_device->CreateDepthStencilState(&dsd, &pDSState);
    assert(SUCCEEDED(hr));

    D3D_device_context->OMSetDepthStencilState(pDSState, 1);

    ID3D11Texture2D* pDepthStencil;
    D3D11_TEXTURE2D_DESC descDepth = {};

    descDepth.Width = 800;
    descDepth.Height = 600;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D32_FLOAT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    D3D_device->CreateTexture2D(&descDepth, nullptr, &pDepthStencil);

    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
    descDSV.Format = DXGI_FORMAT_D32_FLOAT;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    descDSV.Flags = 0;
    D3D_device->CreateDepthStencilView(pDepthStencil, &descDSV, &pDSV);

    D3D_device_context->OMSetRenderTargets(1, backbuffer.GetAddressOf(), pDSV);

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Create Input Layout
    ///////////////////////////////////////////////////////////////////////////////////////////

    //Create the input layout object - 
    D3D11_INPUT_ELEMENT_DESC ied[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },

    };
    OutputDebugStringA("Input element created");

    hr = D3D_device->CreateInputLayout(ied, ARRAYSIZE(ied), VS->GetBufferPointer(), VS->GetBufferSize(), &pLayout);
    assert(SUCCEEDED(hr));

    //Initialise ConstantBuffer
    constantBuffer.Initialize(D3D_device.Get(), D3D_device_context.Get());

    //Initialise Rasterizer State
    rasterizer.Initialise(D3D_device.Get());


    ///////////////////////////////////////////////////////////////////////////////////////////
    // Load Model data
    ///////////////////////////////////////////////////////////////////////////////////////////

    if (bear.Initialize("Model\\bear.glb", this->D3D_device.Get(), this->D3D_device_context.Get(), nullptr, constantBuffer)) {
        OutputDebugStringA("Model Initialized");
    }

    if (Cube.Initialize("Model\\Cube.fbx", this->D3D_device.Get(), this->D3D_device_context.Get(), nullptr, constantBuffer)) {
        OutputDebugStringA("Model Initialized");
    }

    if (D20.Initialize("Model\\D20.fbx", this->D3D_device.Get(), this->D3D_device_context.Get(), nullptr, constantBuffer)) {
        OutputDebugStringA("Model Initialized");
    }

    if (D4.Initialize("Model\\D4.fbx", this->D3D_device.Get(), this->D3D_device_context.Get(), nullptr, constantBuffer)) {
        OutputDebugStringA("Model Initialized");
    }

    if (D8.Initialize("Model\\D8.fbx", this->D3D_device.Get(), this->D3D_device_context.Get(), nullptr, constantBuffer)) {
        OutputDebugStringA("Model Initialized");
    }

    if (D10.Initialize("Model\\D10.fbx", this->D3D_device.Get(), this->D3D_device_context.Get(), nullptr, constantBuffer)) {
        OutputDebugStringA("Model Initialized");
    }

    if (D12.Initialize("Model\\D12.fbx", this->D3D_device.Get(), this->D3D_device_context.Get(), nullptr, constantBuffer)) {
        OutputDebugStringA("Model Initialized");
    }
  
}

void Graphic::BeginFrame(const HWND HWnd)
{
    
    // Set stages and Draw Frame
    //INPUT LAYOUT
    D3D_device_context->IASetInputLayout(pLayout);

    //Input Assembly Stage
    D3D_device_context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    //Vertex and Pixel shader stage
    D3D_device_context->VSSetShader(pVS, 0, 0);
    D3D_device_context->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
    D3D_device_context->PSSetShader(pPS, 0, 0); 
}

void Graphic::Draw(Model model, const float Angle, float x, float y, float z)
{
    const ConstBuffer cb =
    {
        //Matrix must be transposed to be column major, as vertex shader will read matrix as column major 
        XMMatrixTranspose(
        XMMatrixRotationZ(Angle) * XMMatrixRotationX(Angle) * XMMatrixTranslation(x,y,z + 4) * XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 10.0f)
        )
    };

    model.Draw(cb.transform);
}


void Graphic::Present()
{
    swapchain->Present(1, 0);
}

void Graphic::ClearView()
{
    // clear the back buffer to a deep blue
    D3D_device_context->ClearRenderTargetView(this->backbuffer.Get(), rgba{ 0.1f, 0.1f, 1.0f, 1.0f });
    D3D_device_context->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void Graphic::CleanD3D() const
{
    // this is the function that cleans up Direct3D and COM
    const HRESULT hr =  swapchain->SetFullscreenState(FALSE, nullptr);    // switch to windowed mode
    assert(SUCCEEDED(hr));

    // close and release all existing COM objects
    pVS->Release();
    pPS->Release();
    pLayout->Release();
    swapchain->Release();
    backbuffer->Release();
    pDSState->Release();
    pDSV->Release();
    VS->Release();
    PS->Release();
    D3D_device->Release();
    D3D_device_context->Release();
}
