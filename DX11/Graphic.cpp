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
        &D3D_device,
        nullptr,
        &D3D_device_context
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
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;                         // use 32-bit color
    scd.BufferDesc.Width = 0;                                                   // set back buffer width
    scd.BufferDesc.Height = 0;                                                  // set back buffer height
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
        D3D_device,
        &scd,
        &swapchain
    );
    if (FAILED(hr))
    {
        MessageBox(nullptr, "DxgiCreateSwapchain() failed", "Fatal Error", MB_OK);
    }

    OutputDebugStringA("Created Device and SwapChain");
}

void Graphic::DrawTestTriangle(const HWND HWnd, const float Angle )
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    // Create Back Buffer
    ///////////////////////////////////////////////////////////////////////////////////////////

    // Get the address of the back buffer
    ID3D11Texture2D* pBackBuffer;
    HRESULT hr = swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&pBackBuffer));
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
    ID3DBlob* VS, * PS, * error_blob;
    constexpr UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
    hr = D3DCompileFromFile(L"VertexShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0", flags, 0, &VS, &error_blob);
    assert(SUCCEEDED(hr));
    hr = D3DCompileFromFile(L"PixelShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_5_0", flags, 0, &PS, &error_blob);
    assert(SUCCEEDED(hr));

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
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    OutputDebugStringA("Input element created");

    hr = D3D_device->CreateInputLayout(ied, ARRAYSIZE(ied), VS->GetBufferPointer(), VS->GetBufferSize(), &pLayout);
    assert(SUCCEEDED(hr));

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Create Vertex Structure
    ///////////////////////////////////////////////////////////////////////////////////////////

    Vertex OurVertices[] =
    {

        {-1.0f, -1.0f, -1.0f, {1.0f, 0.0f, 0.0f, 1.0f}},
        {1.0f, -1.0f, -1.0f, {1.0f, 0.0f, 0.0f, 1.0f}},
        {-1.0f, 1.0f, -1.0f, {0.0f, 1.0f, 0.0f, 1.0f}},
        {1.0f, 1.0f, -1.0f, {0.0f, 0.0f, 1.0f, 1.0f}},
        
        {-1.0f, -1.0f, 1.0f,  1.0f, 0.0f, 0.0f, 1.0f},
        {1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f},
        {-1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f},
        {1.0f, 1.0f, 1.0f,  1.0f, 0.0f, 1.0f, 1.0f},

    };
    stride = sizeof(Vertex);
    numVerts = sizeof(OurVertices) / stride;
    offset = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Create Vertex Buffer
    ///////////////////////////////////////////////////////////////////////////////////////////

    // create the vertex buffer
    D3D11_BUFFER_DESC bd;

    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.CPUAccessFlags = 0u;
    bd.MiscFlags = 0u;
    bd.ByteWidth = sizeof(OurVertices);
    bd.StructureByteStride = sizeof(Vertex);
    D3D11_SUBRESOURCE_DATA sd = {};
    sd.pSysMem = OurVertices;

    D3D_device->CreateBuffer(&bd, &sd, &pVBuffer);       // Create the buffer



    ///////////////////////////////////////////////////////////////////////////////////////////
    // Create Index Buffer
    ///////////////////////////////////////////////////////////////////////////////////////////

    const unsigned short indices[] =
    {
        0,2,1, 2,3,1,
        1,3,5, 3,7,5,
        2,6,3, 3,6,7, //maybe wrong
        4,5,7, 4,7,6,
        0,4,2, 2,4,6,
        0,1,4, 1,5,4
        
        
    };
    D3D11_BUFFER_DESC ibd;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.Usage = D3D11_USAGE_DEFAULT;
    ibd.CPUAccessFlags = 0u;
    ibd.MiscFlags = 0u;
    ibd.ByteWidth = sizeof(indices);
    ibd.StructureByteStride = sizeof(unsigned short);
    D3D11_SUBRESOURCE_DATA isd = {};
    isd.pSysMem = indices;
    D3D_device->CreateBuffer(&ibd, &isd, &pIBuffer);

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Create Constant Buffer
    ///////////////////////////////////////////////////////////////////////////////////////////

    struct ConstantBuffer
    {
        XMMATRIX transform;
    };

    const ConstantBuffer cb =
    {
        //Matrix must be transposed to be column major, as vertex shader will read matrix as column major 
        XMMatrixTranspose(
        XMMatrixRotationZ(Angle) * XMMatrixRotationX(Angle) *XMMatrixTranslation(0,0,4)* XMMatrixPerspectiveLH(1.0f, 3.0f/4.0f, 0.5f, 10.0f)
        )
    };
    D3D11_BUFFER_DESC cbd;
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.Usage = D3D11_USAGE_DYNAMIC;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbd.MiscFlags = 0u;
    cbd.ByteWidth = sizeof(cb);
    cbd.StructureByteStride = 0u;
    D3D11_SUBRESOURCE_DATA csd = {};
    csd.pSysMem = &cb;

    hr = D3D_device->CreateBuffer(&cbd, &csd, &pCBuffer);
    assert(SUCCEEDED(hr));

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Set stages and Draw Frame
    ///////////////////////////////////////////////////////////////////////////////////////////

    // clear the back buffer to a deep blue
    D3D_device_context->ClearRenderTargetView(backbuffer, rgba(0.1f, 0.1f, 1.0f, 1.0f));

    // Set the viewport
    RECT winRect;
    GetClientRect(HWnd, &winRect);
    const D3D11_VIEWPORT viewport = { 0.0f, 0.0f, static_cast<FLOAT>(winRect.right - winRect.left), static_cast<FLOAT>(winRect.bottom - winRect.top), 0.0f, 1.0f };
    D3D_device_context->RSSetViewports(1, &viewport);

    D3D_device_context->OMSetRenderTargets(1, &backbuffer, nullptr);

    //Input Assembly Stage
    D3D_device_context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    D3D_device_context->IASetInputLayout(pLayout);
    D3D_device_context->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);
    D3D_device_context->IASetIndexBuffer(pIBuffer, DXGI_FORMAT_R16_UINT, 0);

    //Vertex and Pixel shader stage
    D3D_device_context->VSSetShader(pVS, 0, 0);
    D3D_device_context->PSSetShader(pPS, 0, 0);

    D3D_device_context->VSSetConstantBuffers(0, 1, &pCBuffer);

    // draw the vertex buffer to the back buffer
    D3D_device_context->DrawIndexed(ARRAYSIZE(indices), 0, 0);

    // switch the back buffer and the front buffer
    swapchain->Present(1, 0);
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
    pVBuffer->Release();
    swapchain->Release();
    backbuffer->Release();
    D3D_device->Release();
    D3D_device_context->Release();
}
