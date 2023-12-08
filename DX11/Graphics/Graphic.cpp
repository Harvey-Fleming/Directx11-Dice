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
    if (FAILED(hr))
    {
        OutputDebugStringA("Failed to create render target view");
    }
    pBackBuffer->Release();

    

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Create and set Pixel and Vertex Shaders
    ///////////////////////////////////////////////////////////////////////////////////////////
    // 
    //Create the input layout object - 
    D3D11_INPUT_ELEMENT_DESC ied[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 },

    };
    UINT numElements = ARRAYSIZE(ied);

    std::wstring shaderfolder = L"";
    #pragma region - DetermineShaderPath
    if (IsDebuggerPresent() == TRUE)
    {
    #ifdef _DEBUG //Debug Mode
        #ifdef _WIN64 //x64
                shaderfolder = L"..\\x64\\Debug\\";
        #else  //x86 (Win32)
                shaderfolder = L"..\\Debug\\";
        #endif
    #else //Release Mode
        #ifdef _WIN64 //x64
                shaderfolder = L"..\\x64\\Release\\";
        #else  //x86 (Win32)
                shaderfolder = L"..\\Release\\";
        #endif
    #endif
    }
    #pragma endregion   

    //// Compile the vertex and Pixel Shaders
    // Compile the vertex and Pixel Shaders
    if (!vertexShader.Initialize(this->D3D_device, shaderfolder + L"vertexshader.cso", ied, numElements))
        OutputDebugStringA("Failed to initialize vertex Shader");

    if (!pixelShader.Initialize(this->D3D_device, shaderfolder + L"pixelshader.cso"))
        OutputDebugStringA("Failed to initialize vertex Shader");

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
    // Create Sampler State
    ///////////////////////////////////////////////////////////////////////////////////////////

    CD3D11_SAMPLER_DESC samplerDesc(D3D11_DEFAULT);
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

    hr = D3D_device->CreateSamplerState(&samplerDesc, &pSS);
    if (FAILED(hr))
    {
        OutputDebugStringA("Failed to create Sampler state from sampler description");
    }

    //Initialise ConstantBuffer
    constantBuffer.Initialize(D3D_device.Get(), D3D_device_context.Get());

    //Initialise Rasterizer State
    rasterizer.Initialise(D3D_device.Get());


    ///////////////////////////////////////////////////////////////////////////////////////////
    // Load Model data
    ///////////////////////////////////////////////////////////////////////////////////////////
    if (!D6.Initialize("Model\\D6.fbx", this->D3D_device.Get(), this->D3D_device_context.Get(), constantBuffer)) {
        OutputDebugStringA("Failed to Initial Model");
    }
    models.push_back(D6);
    if (!D20.Initialize("Model\\D20.fbx", this->D3D_device.Get(), this->D3D_device_context.Get(), constantBuffer)) {
        OutputDebugStringA("Failed to Initial Model");
    }
    models.push_back(D20);
    if (!D4.Initialize("Model\\D4.fbx", this->D3D_device.Get(), this->D3D_device_context.Get(), constantBuffer)) {
        OutputDebugStringA("Failed to Initial Model");
    }
    models.push_back(D4);
    if (!D8.Initialize("Model\\D8.fbx", this->D3D_device.Get(), this->D3D_device_context.Get(), constantBuffer)) {
        OutputDebugStringA("Failed to Initial Model");
    }
    models.push_back(D8);
    if (!D10.Initialize("Model\\D10.fbx", this->D3D_device.Get(), this->D3D_device_context.Get(), constantBuffer)) {
        OutputDebugStringA("Failed to Initial Model");
    }
    models.push_back(D10);
    if (!D12.Initialize("Model\\D12.fbx", this->D3D_device.Get(), this->D3D_device_context.Get(), constantBuffer)) {
        OutputDebugStringA("Failed to Initial Model");
    }
    models.push_back(D12);

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Setup ImGui
    ///////////////////////////////////////////////////////////////////////////////////////////
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui_ImplWin32_Init(HWnd);
    ImGui_ImplDX11_Init(D3D_device.Get(), D3D_device_context.Get());
    ImGui::StyleColorsDark();
}

void Graphic::BeginFrame(const HWND HWnd)
{
    
    // Set stages and Draw Frame
    //INPUT LAYOUT
    D3D_device_context->IASetInputLayout(vertexShader.GetInputLayout());

    //Input Assembly Stage
    D3D_device_context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    D3D_device_context->RSSetState(rasterizer.Get());

    //Vertex and Pixel shader stage
    D3D_device_context->VSSetShader(vertexShader.GetShader(), 0, 0);
    D3D_device_context->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
    D3D_device_context->PSSetSamplers(0, 1, &pSS);
    D3D_device_context->PSSetShader(pixelShader.GetShader(), 0, 0); 

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    static rgba colorTest;
    static float faceDebug;
    ImGui::SetNextWindowPos(ImVec2(110.0f,400.0f), ImGuiCond_Once);
    ImGui::Begin("Choose Your Fate", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);

    ImGui::Text("Here You Can Change The Current Displayed Dice And It's Color.");               // Display some text (you can use a format strings too)
    
#pragma region - Buttons
    if (ImGui::Button("D4", ImVec2(50, 50)))
    {
        //OutputDebugStringA("4 Button Pressed");
        D6.isActive = false;
        D20.isActive = false;
        D10.isActive = false;
        D8.isActive = false;
        D12.isActive = false;
        ClearView();
        D4.isActive = true;
        rollerHelp.ReRoll(faceDebug);
    }
    ImGui::SameLine(0, 50.0f);
    if (ImGui::Button("D6", ImVec2(50, 50)))
    {
        //OutputDebugStringA("D6 Button Pressed");
        D8.isActive = false;
        D20.isActive = false;
        D10.isActive = false;
        D4.isActive = false;
        D12.isActive = false;
        ClearView();
        D6.isActive = true;
        rollerHelp.ReRoll(faceDebug);

    }
    ImGui::SameLine(0, 50.0f);
    if(ImGui::Button("D8", ImVec2(50, 50)))
    {
        //OutputDebugStringA("D12 Button Pressed");
        D6.isActive = false;
        D20.isActive = false;
        D10.isActive = false;
        D4.isActive = false;
        D12.isActive = false;
        ClearView();
        D8.isActive = true;
        rollerHelp.ReRoll(faceDebug);

    }
    ImGui::SameLine(0, 50.0f);
    if(ImGui::Button("D10", ImVec2(50, 50)))
    {
        //OutputDebugStringA("D12 Button Pressed");
        D6.isActive = false;
        D20.isActive = false;
        D12.isActive = false;
        D4.isActive = false;
        D8.isActive = false;
        ClearView();
        D10.isActive = true;
        rollerHelp.ReRoll(faceDebug);

    }
    ImGui::SameLine(0, 50.0f);
    if (ImGui::Button("D12", ImVec2(50, 50)))
    {
        //OutputDebugStringA("D12 Button Pressed");
        D6.isActive = false;
        D20.isActive = false;
        D10.isActive = false;
        D4.isActive = false;
        D8.isActive = false;
        ClearView();

        D12.isActive = true;
        rollerHelp.ReRoll(faceDebug);

    }
    ImGui::SameLine(0, 50.0f);
    if (ImGui::Button("D20", ImVec2(50, 50))) 
    {
        //OutputDebugStringA("D20 Button Pressed");
        D6.isActive = false;
        D12.isActive = false;
        D10.isActive = false;
        D4.isActive = false;
        D8.isActive = false;
        ClearView();      
        D20.isActive = true;
        rollerHelp.ReRoll(faceDebug);

    }
#pragma endregion  

    ImGui::ColorEdit3("", (float*)&colorTest);   // Edit 3 floats representing a color
    ImGui::SameLine();
    ImGui::Button("Apply Color");                           // Buttons return true when clicked (most widgets return true when edited/activated)

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::End();


    ImGui::SetNextWindowPos(ImVec2(600.0f, 300.0f), ImGuiCond_Once);
    ImGui::Begin("Dice Debug Menu", NULL, ImGuiWindowFlags_NoCollapse);

    ImGui::DragFloat("##", &faceDebug, 0.25f, 1.0f, 20.0f);

    ImGui::DragFloat3("", (float*)&rotOffset, 0.5f, -360.0f, 360.0f);   // Edit 3 floats representing a color

    ImGui::End();

    ImGui::Render();
    rollerHelp.CalculateAngle();
}

void Graphic::Draw(Model model, float x, float y, float z, float xScale, float yScale, float zScale)
{
    static XMVECTOR yAxis = { 0,1,0 };
    static XMVECTOR xAxis = { 1,0,0 };
    static XMVECTOR zAxis = { 0,0,1 };
    const ConstBuffer cb =
    {
        //Matrix must be transposed to be column major, as vertex shader will read matrix as column major 
        XMMatrixTranspose(
        XMMatrixRotationAxis(xAxis, rollerHelp.GetAngleX() + rotOffset[0]) * XMMatrixRotationAxis(yAxis, rollerHelp.GetAngleY() + rotOffset[1]) * XMMatrixRotationAxis(zAxis, rollerHelp.GetAngleZ() + rotOffset[2]) * XMMatrixScaling(xScale,yScale,zScale) * XMMatrixTranslation(x,y,z + 4) * XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 10.0f)
        )
    };

    //output current rotation
    std::ostringstream ss;
    ss << rollerHelp.GetAngleX();
    ss << ", ";
    ss << rollerHelp.GetAngleY();
    ss << ", ";
    ss << rollerHelp.GetAngleZ();
    ss << "\n";
    std::string s(ss.str());
    OutputDebugStringW(StringHelper::StringToWide(s.c_str()).c_str());

    if (model.isActive)
    {
        model.Draw(cb.transform);
    }
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
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

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    // close and release all existing COM objects
    swapchain->Release();
    backbuffer->Release();
    pDSState->Release();
    pDSV->Release();
    pSS->Release();
    D3D_device->Release();
    D3D_device_context->Release();
}
