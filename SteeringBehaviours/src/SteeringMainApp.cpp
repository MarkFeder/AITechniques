#pragma warning (disable:4786)

#include "Steering-Behaviours/Public/Constants.h"
#include "Steering-Behaviours/Public/GameWorld.h"
#include "Steering-Behaviours/Public/ParamLoader.h"
#include "Steering-Behaviours/Public/Resource.h"
#include "Common/Public/Misc/Cgdi.h"
#include "Common/Public/Misc/Utils.h"
#include "Common/Public/Time/PrecisionTimer.h"
#include "Common/Public/Misc/WindowsUtils.h"

#include <windows.h>
#include <time.h>

//--------------------------- Globals ------------------------------------------
//------------------------------------------------------------------------------

char* g_szApplicationName = "Steering Behaviours";
char* g_szWindowClassName = "MyWindowClass";

GameWorld* g_gameWorld;

//--------------------------- WindowsProc --------------------------------------
// This is the callback function which handles all the windows messages
//------------------------------------------------------------------------------

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// These hold the dimensions of the client window area
	static int cxClient, cyClient;

	// Used to create the back buffer
	static HDC hdcBackBuffer;
	static HBITMAP hBitmap;
	static HBITMAP hOldBitmap;

	switch (msg)
	{
		case WM_CREATE:
		{
			// To get the size of the client window, first we need to create a RECT and then ask
			// Windows to fill in our RECT structure with the client window size. Then, we assign to
			// cxClient and cyClient accordingly
			RECT rect;
			GetClientRect(hwnd, &rect);

			cxClient = rect.right;
			cyClient = rect.bottom;

			// Seed random number generator
			srand((unsigned int)time(NULL));

			// Create a surface to render to (backBuffer)

			// Create a memory device context
			hdcBackBuffer = CreateCompatibleDC(NULL);

			// Get the DC for the front buffer
			HDC hdc = GetDC(hwnd);

			hBitmap = CreateCompatibleBitmap(hdc, cxClient, cyClient);

			// Select the bitmap into the memory device context
			hOldBitmap = (HBITMAP)SelectObject(hdcBackBuffer, hBitmap);

			// Don't forget to release the DC
			ReleaseDC(hwnd, hdc);

			g_gameWorld = new GameWorld(cxClient, cyClient);

			ChangeMenuState(hwnd, IDR_PRIORITIZED, MFS_CHECKED);
			ChangeMenuState(hwnd, ID_VIEW_FPS, MFS_CHECKED);
		}

		break;

		case WM_COMMAND:
		{
			g_gameWorld->HandleMenuItems(wParam, hwnd);
		}

		break;

		case WM_LBUTTONUP:
		{
			g_gameWorld->SetCrosshair(MAKEPOINTS(lParam));
		}

		break;

		case WM_KEYUP:
		{
			if (wParam == VK_ESCAPE)
			{
				SendMessage(hwnd, WM_DESTROY, NULL, NULL);
			}
			else if (wParam == 'R')
			{
				delete g_gameWorld;

				g_gameWorld = new GameWorld(cxClient, cyClient);
			}
			else
			{
				// Handle any others
				g_gameWorld->HandleKeyPresses(wParam);
			}
		}

		break;

		case WM_PAINT:
		{
			PAINTSTRUCT ps;

			BeginPaint(hwnd, &ps);

			// Fill our backbuffer with white
			BitBlt(hdcBackBuffer, 0, 0, cxClient, cyClient, NULL, NULL, NULL, WHITENESS);

			gdi->StartDrawing(hdcBackBuffer);

			g_gameWorld->Render();

			gdi->StopDrawing(hdcBackBuffer);

			// Now blit backbuffer to front
			BitBlt(ps.hdc, 0, 0, cxClient, cyClient, hdcBackBuffer, 0, 0, SRCCOPY);

			EndPaint(hwnd, &ps);
		}

		break;

		case WM_DESTROY:
		{
			// Clean up our backbuffer objects
			SelectObject(hdcBackBuffer, hOldBitmap);

			DeleteDC(hdcBackBuffer);
			DeleteObject(hBitmap);

			// Kill the app, this sends a WM_QUIT message
			PostQuitMessage(0);
		}

		break;
	}

	// This is where all the messages not specifically handled by our winproc are sent to be processed
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

//--------------------------- WinMain ------------------------------------------
// The entry point of the windows program
//------------------------------------------------------------------------------

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow)
{
	// Handle to our window
	HWND hWnd;

	// Our window class structure
	WNDCLASSEX winClass;

	// First fill in the window class structure
	winClass.cbSize = sizeof(WNDCLASSEX);
	winClass.style = CS_HREDRAW | CS_VREDRAW;
	winClass.lpfnWndProc = WindowProc;
	winClass.cbClsExtra = 0;
	winClass.cbWndExtra = 0;
	winClass.hInstance = hInstance;
	winClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	winClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	winClass.hbrBackground = NULL;
	winClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	winClass.lpszClassName = g_szWindowClassName;
	winClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	// Register the window class
	if (!RegisterClassEx(&winClass))
	{
		MessageBox(NULL, "Registration Failed!", "Error", 0);

		// Exit the application
		return 0;
	}

	// Create the window and assign its ID to hwnd
	hWnd = CreateWindowEx(
		NULL, // Extended style
		g_szWindowClassName, // Window class name
		g_szApplicationName, // Window caption
		WS_OVERLAPPED | WS_VISIBLE | WS_CAPTION | WS_SYSMENU,
		GetSystemMetrics(SM_CXSCREEN) / 2 - CONST_WINDOW_WIDTH / 2,
		GetSystemMetrics(SM_CYSCREEN) / 2 - CONST_WINDOW_HEIGHT / 2,
		CONST_WINDOW_WIDTH, // Initial x size
		CONST_WINDOW_HEIGHT, // Initial y size
		NULL, // Parent window handle
		NULL, // Window menu handle
		hInstance, // Program instance handle
		NULL // Creation parameters
	);

	// Make sure the window creation has gone OK
	if (!hWnd)
	{
		MessageBox(NULL, "CreateWindowEx Failed!", "Error!", 0);
	}

	// Make the window visible
	ShowWindow(hWnd, iCmdShow);
	UpdateWindow(hWnd);

	// Enter the message loop
	bool bDone = false;

	// Create a timer
	PrecisionTimer timer;

	timer.SmoothUpdatesOn();

	// Start the timer
	timer.Start();

	MSG msg;

	while (!bDone)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				// Stop loop if it's a quit message
				bDone = true;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		if (msg.message != WM_QUIT)
		{
			// Update
			g_gameWorld->Update(timer.TimeElapsed());

			// Render
			RedrawWindow(hWnd, false);

			Sleep(2);
		}
	}

	delete g_gameWorld;

	UnregisterClass(g_szWindowClassName, winClass.hInstance);

	return msg.wParam;
}