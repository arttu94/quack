#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <iostream>
#include <string>
#include <fstream>
#include <cstdio>
#include <vector>
#include <Windows.h>
#include <mmsystem.h>

using namespace std;

HHOOK g_Hook;
HHOOK k_Hook;
HANDLE g_evExit;

HCURSOR hcur;

LRESULT CALLBACK LowLevelKeyboardProc(int code, WPARAM wParam, LPARAM lParam)
{
	if (code == HC_ACTION)
	{
		const char *msg;
		char msg_buff[128];
		switch (wParam)
		{
		case WM_KEYDOWN:
			msg = "Keydown";
			break;
		case WM_KEYUP:
			msg = "Keyup";
			break;
		default:
			msg_buff;
			break;
		}

		const MSLLHOOKSTRUCT *p = reinterpret_cast<const MSLLHOOKSTRUCT*>(lParam);
		//cout << msg << " - [" << p->pt.x << ',' << p->pt.y << ']' << endl;

		static bool keydown = false;
		static bool keyup = false;
		switch (wParam)
		{
		case WM_KEYDOWN: keydown = true; break;
		case WM_KEYUP:   keydown = false; break;
		}//switch

		if (keydown)
		{
			//sndPlaySound(TEXT("data/quack.wav"), SND_ASYNC);
			PlaySound(TEXT("data/quack.wav"), NULL, SND_ASYNC);
		}
		else if(keyup)
		{
			PlaySound(NULL, NULL, SND_ASYNC);
		}
	}
	return CallNextHookEx(g_Hook, code, wParam, lParam);
}

LRESULT CALLBACK LowLevelMouseProc(int code, WPARAM wParam, LPARAM lParam)
{

	if (code == HC_ACTION)
	{
		const char *msg;
		char msg_buff[128];
		switch (wParam)
		{
		case WM_LBUTTONDOWN: msg = "WM_LBUTTONDOWN";
			break;
		case WM_LBUTTONUP: msg = "WM_LBUTTONUP";
			break;
		case WM_RBUTTONDOWN: msg = "WM_RBUTTONDOWN";
			break;
		case WM_RBUTTONUP: msg = "WM_RBUTTONUP";
			break;
		default:
			//sprintf(msg_buff, "Unknown msg: %u", wParam);
			msg = msg_buff;
			break;
		}//switch

		const MSLLHOOKSTRUCT *p =
			reinterpret_cast<const MSLLHOOKSTRUCT*>(lParam);
		//cout << msg << " - [" << p->pt.x << ',' << p->pt.y << ']' << endl;

		static bool left_down = false;
		static bool right_down = false;
		switch (wParam)
		{
		case WM_LBUTTONDOWN: left_down = true; break;
		case WM_LBUTTONUP:   left_down = false; break;
		case WM_RBUTTONDOWN: right_down = true; break;
		case WM_RBUTTONUP:   right_down = false; break;
		}//switch

		if (left_down && right_down)
			SetEvent(g_evExit);
	}//if

	return CallNextHookEx(g_Hook, code, wParam, lParam);
}//LowLevelMouseProc

int main()
{
	g_evExit = CreateEvent(0, TRUE, FALSE, 0);
	if (!g_evExit)
	{
		cerr << "CreateEvent failed, le = " << GetLastError() << endl;
		return 1;
	}

	g_Hook = SetWindowsHookEx(WH_MOUSE_LL, &LowLevelMouseProc, GetModuleHandle(0), 0);
	if (!g_Hook)
	{
		SystemParametersInfo(SPI_SETCURSORS, 0, NULL, 0);
		cerr << "SetWindowsHookEx() failed, le = " << GetLastError() << endl;
		return 1;
	}

	k_Hook = SetWindowsHookEx(WH_KEYBOARD_LL, &LowLevelKeyboardProc, GetModuleHandle(0), 0);
	if (!k_Hook)
	{
		SystemParametersInfo(SPI_SETCURSORS, 0, NULL, 0);
		return 1;
	}

	cout << "Press both left and right mouse buttons to exit..." << endl;

	//ShowWindow(GetConsoleWindow(), SW_HIDE);

	/* 
		this changes the cursor to a duck but now it doesn't revert
		to the old default cursor, idk why. so it's gonna be commented until 
		I figure out why it doesn't change back.
	*/
	//hcur = LoadCursorFromFile(L"data/duck.cur");
	//SetSystemCursor(hcur, 32512/*OCR_NORMAL*/);

	MSG msg;

	DWORD status;

	while (1)
	{
		while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
			DispatchMessage(&msg);

		status = MsgWaitForMultipleObjects(1, &g_evExit, FALSE,
			INFINITE, QS_ALLINPUT);
		if (status == (WAIT_OBJECT_0 + 1))
		{
			// there are messages to process, eat em up
			continue;
		}
		else
		{
			SystemParametersInfo(SPI_SETCURSORS, 0, NULL, 0);
			// assume g_evExit is signaled
			break;
		}
	}

	//cout << "Exiting..." << endl;
	SystemParametersInfo(SPI_SETCURSORS, 0, NULL, 0);

	UnhookWindowsHookEx(g_Hook);
	UnhookWindowsHookEx(k_Hook);
	CloseHandle(g_evExit);

	return 0;
}//main