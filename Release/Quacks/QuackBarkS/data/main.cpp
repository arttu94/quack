#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <iostream>
#include <cstdio>
#include <Windows.h>
#include <mmsystem.h>
#include "..\include\audiere.h"
using namespace std;
using namespace audiere;

HHOOK g_Hook;
HANDLE g_evExit;

AudioDevicePtr device;
OutputStreamPtr sound;

LRESULT CALLBACK LowLevelMouseProc(int code, WPARAM wParam, LPARAM lParam)
{

	if (code == HC_ACTION)
	{
		const char *msg;
		char msg_buff[128];
		switch (wParam)
		{
		case WM_LBUTTONDOWN: msg = "WM_LBUTTONDOWN"; PlaySound(TEXT("Quack(1).WAV"), NULL, SND_ASYNC); break;
		case WM_LBUTTONUP: msg = "WM_LBUTTONUP"; /*PlaySound(NULL, NULL, SND_ASYNC);*/ break;
		/*case WM_MOUSEMOVE: msg = "WM_MOUSEMOVE"; break;
		case WM_MOUSEWHEEL: msg = "WM_MOUSEWHEEL"; break;
		case WM_MOUSEHWHEEL: msg = "WM_MOUSEHWHEEL"; break;*/
		case WM_RBUTTONDOWN: msg = "WM_RBUTTONDOWN"; PlaySound(TEXT("Quack(1).WAV"), NULL, SND_ASYNC);  break;
		case WM_RBUTTONUP: msg = "WM_RBUTTONUP"; break;
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

		//if (left_down || right_down)
			//PlaySound(TEXT("Quack(1).WAV"), NULL, SND_ASYNC);
			//sound->play();

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
	}//if

	g_Hook = SetWindowsHookEx(WH_MOUSE_LL, &LowLevelMouseProc,
		GetModuleHandle(0), 0);
	if (!g_Hook)
	{
		cerr << "SetWindowsHookEx() failed, le = " << GetLastError() << endl;
		return 1;
	}//if

	cout << "Press both left and right mouse buttons to exit..." << endl;

	FreeConsole();

	MSG msg;
	DWORD status;

	/*device = (OpenDevice());
	if (!device)
		return 0;
	sound = (OpenSound(device, "quack.ogg", true));
	if (!sound)
		return 0;*/

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
		}//if
		else
		{
			// assume g_evExit is signaled
			break;
		}//else
	}//while

	//cout << "Exiting..." << endl;
	UnhookWindowsHookEx(g_Hook);
	CloseHandle(g_evExit);
	return 0;
}//main


//#include <Windows.h>
//#include <mmsystem.h>
//#include <iostream>
//using namespace std;
//
//#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
//#define KEY_UP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)
//
//int main()
//{
//	//FreeConsole();
//	while (1) {
//		//for (char button = 0; button < 256; button++) {
//		//	if (GetAsyncKeyState(button) & 0x8000) {
//		//		// Button Pressed
//		//		PlaySound(TEXT("Quack(1).WAV"), NULL, SND_FILENAME);
//		//	}
//		//}
//		if (GetAsyncKeyState(VK_DOWN) & 0x8000)
//		{
//			PlaySound(TEXT("Quack(1).WAV"), NULL, SND_FILENAME);
//		}
//
//		if(KEY_DOWN(VK_LBUTTON) || KEY_DOWN(VK_RBUTTON))
//			PlaySound(TEXT("Quack(1).WAV"), NULL, SND_FILENAME);
//		//Sleep(1);
//	}
//	//getchar(); // wait until key is pressed
//	//return EXIT_SUCCESS;
//}