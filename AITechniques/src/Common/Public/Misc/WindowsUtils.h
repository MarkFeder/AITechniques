#pragma once
#pragma warning (disable:4786)

#include <Windows.h>
#include <string>

struct Vector2D;

// Macro to detect keypresses
#define KEYDOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define WAS_KEY_PRESSED(vk_code) ((GetKeyState(vk_code) & 0x8000) != 0)
#define IS_KEY_PRESSED(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) != 0)

// Call this to refresh the client window
inline void RedrawWindow(HWND hwnd, bool redrawBackground = true)
{
	InvalidateRect(hwnd, NULL, redrawBackground);
	UpdateWindow(hwnd);
}

// Call this to refresh the client window
inline void RedrawWindowRect(HWND hwnd, bool redrawBackground, RECT& redrawArea)
{
	InvalidateRect(hwnd, &redrawArea, redrawBackground);
	UpdateWindow(hwnd);
}

// Changes the state of a menu item given the item identifier, the desired
// state and the HWND of the menu owner
void ChangeMenuState(HWND hwnd, UINT menuItem, UINT state);

// If b is true MenuItem is checked, otherwise it is unchecked
void CheckMenuItemAppropriately(HWND hwnd, UINT menuItem, bool b);

// This is a replacement for the StringCchLength function found in the platform SDK.
// See MSDN for details. Only ever used for checkking toolbar strings.
bool CheckBufferLength(char* buff, int maxLength, unsigned int& bufferLength);

void ErrorBox(std::string& msg);
void ErrorBox(char* msg);

// Gets the coordinates of the cursor relative to an active window
Vector2D GetClientCursorPosition();

// Gets the coordinates of the cursor relative to an active window
Vector2D GetClientCursorPosition(HWND hwnd);

// Function that open a common dialog box to grab a filename
void FileInitialize(HWND hwnd,
	OPENFILENAME& ofn,
	const std::string& defaultTypeDescription,
	const std::string& defaultFileExtension);

BOOL FileOpenDialog(HWND hwnd,
	PTSTR pstrFileName,
	PTSTR pstrTitleName,
	const std::string& defaultFileTypeDescription,
	const std::string& defaultFileExtension);

BOOL FileSaveDialog(HWND hwnd,
	PTSTR pstrFileName,
	PTSTR pstrTitleName,
	const std::string& defaultFileTypeDescription,
	const std::string& defaultFileExtension);

// Call this to resize the specified window to the specified size
void ResizeWindow(HWND hwnd, int cx, int cy);

int GetWindowHeight(HWND hwnd);
int GetWindowWidth(HWND hwnd);
