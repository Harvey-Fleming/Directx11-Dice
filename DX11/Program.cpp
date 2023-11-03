// include the basic windows header files and the Direct3D header files
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <sstream>
#include <istream>

#include "Graphic.h"
#include "ChiliTimer.h"

ChiliTimer timer;
// WindowProc function prototype
LRESULT CALLBACK window_proc(HWND h_wnd, UINT message, WPARAM w_param, LPARAM l_param);

// define the screen resolution
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600



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
                                     "Our First Direct3D Program",
                                     WS_OVERLAPPEDWINDOW,
                                     300,
                                     300,
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
        graphic->DrawTestTriangle(hWnd, timer.Peek());
    }

    // clean up DirectX and COM
    OutputDebugStringA("Cleanup D3d");
    
    graphic->CleanD3D();
    return static_cast<int>(msg.wParam);
}


// this is the main message handler for the program
LRESULT CALLBACK window_proc(const HWND h_wnd, const UINT message, const WPARAM w_param, const LPARAM l_param)
{
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


