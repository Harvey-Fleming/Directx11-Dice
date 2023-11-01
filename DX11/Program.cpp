// include the basic windows header files and the Direct3D header files
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <sstream>
#include "Graphic.h"

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

    Graphic* graphic = new Graphic(hWnd);
    OutputDebugStringA("Began Initialization of Direct 3D");
    
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
        graphic->DrawTestTriangle(hWnd, 0);
    }

    // clean up DirectX and COM
    OutputDebugStringA("Cleanup D3d");
    
    graphic->CleanD3D();
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


