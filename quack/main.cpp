#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <iostream>
#include <string>
#include <fstream>
#include <cstdio>
#include <vector>
#include <Windows.h>
#include <mmsystem.h>

const int MAX_WORD_SIZE = 30;

using namespace std;

HHOOK g_Hook;
HHOOK k_Hook;
HANDLE g_evExit;

HCURSOR hcur;

char keyboardInputBuffer[MAX_WORD_SIZE];
int keyboardInputBufferIndex;

vector<string> vWords;

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

		const MSLLHOOKSTRUCT *p =
			reinterpret_cast<const MSLLHOOKSTRUCT*>(lParam);
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
			keyboardInputBuffer[keyboardInputBufferIndex] = static_cast<char>(p->pt.x);
			keyboardInputBufferIndex = (keyboardInputBufferIndex + 1) % MAX_WORD_SIZE;

			//system("cls");
			for (int i = 0; i < MAX_WORD_SIZE; i++)
			{
				cout << keyboardInputBuffer[i] << " ";
			}

			if (
				p->pt.x == 32 || //spacebar
				p->pt.x == 13 || //return-enter
				p->pt.x == 8 || //backspace
				p->pt.x == 160 || //shift
				p->pt.x == 161 || //shift
				p->pt.x == 162    //ctrl
				//p->pt.x == 66 || //b
				//p->pt.x == 65 || //a
				//p->pt.x == 82 || //r
				//p->pt.x == 75 || //k
				//p->pt.x == 69 || //e
				//p->pt.x == 73 || //i
				//p->pt.x == 79 || //o
				//p->pt.x == 85
				)   //u
			{
				sndPlaySound(TEXT("data/bark.wav"), SND_ASYNC);
			}
		}
		else if (!keyup)
		{
			//PlaySound(NULL, NULL, SND_ASYNC);
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
		//cerr << "CreateEvent failed, le = " << GetLastError() << endl;
		return 1;
	}//if

	//string line;
	//ifstream mFile("ini.txt");
	//if(!mFile.is_open())
	//{
	//	MessageBoxA(nullptr, (LPCSTR)"ini file could not be opened, closing quacker", (LPCSTR)"Error loading ini.txt", MB_OK);
	//	return 1;
	//}
	//else
	//{
	//	while (getline(mFile, line))
	//	{
	//		int endOfFirstFind;
	//		endOfFirstFind = line.find(" ");
	//		string tempWord;
	//		string tempFilename;
	//		for (int i = 0; i < endOfFirstFind; i++)
	//		{
	//			tempWord.push_back(line[i]);
	//		}
	//		cout << tempWord << " ";
	//		for (int i = (int)endOfFirstFind + 1; i < line.size(); i++)
	//		{
	//			tempFilename.push_back(line[i]);
	//		}
	//		cout << tempFilename << endl;
	//		CreateSound(tempFilename, tempWord);
	//		//CreatefmodSound(tempFilename, tempWord);
	//	}
	//	mFile.close();
	//}

	g_Hook = SetWindowsHookEx(WH_MOUSE_LL, &LowLevelMouseProc, GetModuleHandle(0), 0);
	if (!g_Hook)
	{
		//SystemParametersInfo(SPI_SETCURSORS, 0, NULL, 0);
		//cerr << "SetWindowsHookEx() failed, le = " << GetLastError() << endl;
		return 1;
	}//if

	k_Hook = SetWindowsHookEx(WH_KEYBOARD_LL, &LowLevelKeyboardProc, GetModuleHandle(0), 0);
	if (!k_Hook)
	{
		//SystemParametersInfo(SPI_SETCURSORS, 0, NULL, 0);
		return 1;
	}

	//cout << "Press both left and right mouse buttons to exit..." << endl;

	//FreeConsole();

	for (int i = 0; i < MAX_WORD_SIZE; i++)
	{
		keyboardInputBuffer[i] = NULL;
	}
	keyboardInputBufferIndex = 0;

	//hcur = LoadCursorFromFile(L"C:\\windows\\cursors\\duck.cur");
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
		}//if
		else
		{
			SystemParametersInfo(SPI_SETCURSORS, 0, NULL, 0);
			// assume g_evExit is signaled
			break;
		}//else
	}//while

	//cout << "Exiting..." << endl;
	//SystemParametersInfo(SPI_SETCURSORS, 0, NULL, 0);

	UnhookWindowsHookEx(g_Hook);
	UnhookWindowsHookEx(k_Hook);
	CloseHandle(g_evExit);
	return 0;
}//main