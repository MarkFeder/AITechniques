#include "Common/Public/Misc/WindowsUtils.h"
#include "Common/Public/2D/Vector2D.h"
#include "Common/Public/Misc/Utils.h"
#include "Common/Public/Misc/StreamUtils.h"

//---------------------- ChangeMenuState ---------------------------------
//  Changes the state of a menu item given the item identifier, the 
//  desired state and the HWND of the menu owner
//------------------------------------------------------------------------

void ChangeMenuState(HWND hwnd, UINT menuItem, UINT state)
{
	MENUITEMINFO mi;

	mi.cbSize = sizeof(MENUITEMINFO);
	mi.fMask = MIIM_STATE;
	mi.fState = state;

	SetMenuItemInfo(GetMenu(hwnd), menuItem, false, &mi);
	DrawMenuBar(hwnd);
}

//---------------------- CheckMenuItemAppropriately ----------------------
// If b is true, MenuItem is checked, otherwise it is unchecked
//------------------------------------------------------------------------

void CheckMenuItemAppropriately(HWND hwnd, UINT menuItem, bool b)
{
	if (b)
		ChangeMenuState(hwnd, menuItem, MFS_CHECKED);
	else
		ChangeMenuState(hwnd, menuItem, MFS_UNCHECKED);
}

//---------------------- CheckBufferLength -------------------------------
// This a replacement for the StringCchLength function found in the platform
// SDK. See MSDN for details. Only ever used for checking toolbar strings
//------------------------------------------------------------------------

bool CheckBufferLength(char* buff, int maxLength, unsigned int& bufferLength)
{
	std::string s = ttos(buff);
	bufferLength = s.length();

	if (bufferLength > maxLength)
	{
		bufferLength = 0;
		return false;
	}

	return true;
}

void ErrorBox(std::string& msg)
{
	MessageBox(NULL, msg.c_str(), "Error", MB_OK);
}

void ErrorBox(char* msg)
{
	MessageBox(NULL, msg, "Error", MB_OK);
}

//---------------------- GetClientCursorPosition ------------------------
// Gets the coordinates of the cursor relative to an active window
//------------------------------------------------------------------------

Vector2D GetClientCursorPosition()
{
	POINT mousePos;

	GetCursorPos(&mousePos);

	ScreenToClient(GetActiveWindow(), &mousePos);

	return POINTtoVector(mousePos);
}

Vector2D GetClientCursorPosition(HWND hwnd)
{
	POINT mousePos;

	GetCursorPos(&mousePos);

	ScreenToClient(hwnd, &mousePos);

	return POINTtoVector(mousePos);
}

//------------------------------------------------------------------------
// The following 3 functions enable the client to use the file 
// dialog common control
//------------------------------------------------------------------------

void FileInitialize(HWND hwnd, OPENFILENAME& ofn, const std::string& defaultTypeDescription, const std::string& defaultFileExtension)
{
	std::string filter = defaultTypeDescription + '\0' + "*." + defaultFileExtension + '\0' +
		"All Files (*.*)" + '\0' + "*.*" + '\0' + '\0';

	static TCHAR szFilter[255];

	for (unsigned int i = 0; i < filter.size(); ++i)
	{
		szFilter[i] = filter.at(i);
	}

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.hInstance = NULL;
	ofn.lpstrFilter = szFilter;
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter = 0;
	ofn.nFilterIndex = 0;
	ofn.lpstrFile = NULL;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = MAX_PATH;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrTitle = NULL;
	ofn.Flags = 0;
	ofn.nFileOffset = 0;
	ofn.nFileExtension = 0;
	ofn.lpstrDefExt = defaultFileExtension.c_str();
	ofn.lCustData = 0L;
	ofn.lpfnHook = NULL;
	ofn.lpTemplateName = NULL;
}

BOOL FileOpenDialog(HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName, const std::string& defaultFileTypeDescription, const std::string& defaultFileExtension)
{
	OPENFILENAME ofn;

	FileInitialize(hwnd, ofn, defaultFileTypeDescription, defaultFileExtension);

	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = pstrFileName;
	ofn.lpstrFileTitle = pstrTitleName;
	ofn.Flags = OFN_HIDEREADONLY | OFN_CREATEPROMPT;

	return GetOpenFileName(&ofn);
}

BOOL FileSaveDialog(HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName, const std::string& defaultFileTypeDescription, const std::string& defaultFileExtension)
{
	OPENFILENAME ofn;

	FileInitialize(hwnd, ofn, defaultFileTypeDescription, defaultFileExtension);

	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = pstrFileName;
	ofn.lpstrFileTitle = pstrTitleName;
	ofn.Flags = OFN_OVERWRITEPROMPT;

	return GetSaveFileName(&ofn);
}

//---------------------- ResizeWindow ------------------------------------
// Call this to resize the active window to the specified size
//------------------------------------------------------------------------

void ResizeWindow(HWND hwnd, int cx, int cy)
{
	// Does this window have a menu. If so, set a flag to true
	HMENU hwndMenu = GetMenu(hwnd);
	bool bMenu = false;
	if (hwndMenu) bMenu = true;

	// Create a rect of the desired window size
	RECT desiredSize;
	desiredSize.left = 0;
	desiredSize.top = 0;
	desiredSize.right = cx;
	desiredSize.bottom = cy;

	// Determine the size the window should be given the desired client area
	AdjustWindowRectEx(&desiredSize, WS_OVERLAPPED | WS_VISIBLE | WS_CAPTION | WS_SYSMENU, bMenu, NULL);

	// Resize the window to fit
	SetWindowPos(hwnd, 
		NULL,
		(GetSystemMetrics(SM_CXSCREEN) / 2) - (cx / 2),
		(GetSystemMetrics(SM_CYSCREEN) / 2) - (cy / 2),
		desiredSize.right,
		desiredSize.bottom,
		SWP_NOZORDER);
}

//---------------------- GetWindowHeight ---------------------------------
//------------------------------------------------------------------------

int GetWindowHeight(HWND hwnd)
{
	if (hwnd == 0) return 0;

	RECT windowRect;

	GetWindowRect(hwnd, &windowRect);

	return windowRect.bottom - windowRect.top;
}

//---------------------- GetWindowWidth ----------------------------------
//------------------------------------------------------------------------

int GetWindowWidth(HWND hwnd)
{
	if (hwnd == 0) return 0;

	RECT windowRect;

	GetWindowRect(hwnd, &windowRect);

	return windowRect.right - windowRect.left;
}
