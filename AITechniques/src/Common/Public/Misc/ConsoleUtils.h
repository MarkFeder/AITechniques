#pragma once

#include <windows.h>
#include <conio.h>
#include <iostream>

inline void SetTextColor(WORD colors)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleTextAttribute(hConsole, colors);
}

inline void PressAnyKeyToContinue()
{
	// Change text color to white
	SetTextColor(FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN);

	std::cout << "\n\nPress any key to continue" << std::endl;

	while (!_kbhit()) {}

	return;
}