#include "Program.h"


ChiliTimer timer;
// WindowProc function prototype
LRESULT CALLBACK window_proc(HWND h_wnd, UINT message, WPARAM w_param, LPARAM l_param);


// Main entry point for the Windows program
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
#pragma region WindowStuff

    WNDCLASSEX wc;

    ZeroMemory(&wc, sizeof(WNDCLASSEX)); //Initialize wc by clearing our any existing used memory

    wc.cbSize = sizeof(WNDCLASSEX); //Specifies the Size of the Structure
    wc.style = CS_HREDRAW | CS_VREDRAW; //Style of window class we want
    wc.lpfnWndProc = window_proc; //Defines what method will run
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    //wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszClassName = "WindowClass";

    RegisterClassEx(&wc);

    RECT wr = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

    const HWND hWnd = CreateWindowEx(NULL,
                                     "WindowClass",
                                     "Harvey's Dice Roller",
                                     WS_OVERLAPPEDWINDOW,
                                     CW_USEDEFAULT,
                                     CW_USEDEFAULT,
                                     wr.right - wr.left,
                                     wr.bottom - wr.top,
                                     nullptr,
                                     nullptr,
                                     hInstance,
                                     nullptr);

    ShowWindow(hWnd, nCmdShow);
#pragma endregion


    auto* graphic = new Graphic(hWnd);
    OutputDebugStringA("Began Initialization of Direct 3D");
    

    // enter the main loop:

    MSG msg;

    while (TRUE)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT)

                break;
        }
        
        graphic->ClearView(graphic->backColour);
        graphic->BeginFrame(hWnd);

        graphic->Draw(graphic->D6, 0, 0.5f, 0, 1, 1, 1);
        graphic->Draw(graphic->D20, 0, 0.5f, 0, 1, 1, 1);
        graphic->Draw(graphic->D12, 0, 0.5f, 0, 1, 1, 1);
        graphic->Draw(graphic->D4, 0, 0.5f, 0, 1, 1, 1);
        graphic->Draw(graphic->D8, 0, 0.5f, 0, 1, 1, 1);
        graphic->Draw(graphic->D10, 0, 0.5f, 0, 1, 1, 1);

        graphic->Present();
    }

    // clean up DirectX and COM
    OutputDebugStringA("Cleanup D3d");
    
    graphic->CleanD3D();

    return static_cast<int>(msg.wParam);
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
// this is the main message handler for the program
LRESULT CALLBACK window_proc(const HWND h_wnd, const UINT message, const WPARAM w_param, const LPARAM l_param)
{
    if (ImGui_ImplWin32_WndProcHandler(h_wnd, message, w_param, l_param))
        return true;

    switch (message)  // NOLINT(hicpp-multiway-paths-covered)
    {
        case WM_DESTROY:
            {
            PostQuitMessage(0);
            return 0;
        }
    default: return DefWindowProc(h_wnd, message, w_param, l_param);
    }
}


