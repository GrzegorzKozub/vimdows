#include "stdafx.h"

#include <sstream>
#include <string>
#include <cstdint>

#include <windows.h>
#include <shellscalingapi.h>

using namespace std;

class Screen {
private:

	HWND window = NULL;

	static BOOL CALLBACK EnumThreadWndProc(HWND hwnd, LPARAM lParam) {
		HWND* hwndPtr = (HWND*)lParam;
		if (GetParent(hwnd)) {
			*hwndPtr = NULL;
			return true;
		}
		else {
			*hwndPtr = hwnd;
			return false;
		}
	}

public:

	Screen() {
		EnumThreadWindows(GetCurrentThreadId(), EnumThreadWndProc, (LPARAM)&window);
	}

	char* GetScreen() {
		std::stringstream screen;

		HWND window = GetForegroundWindow();
		HMONITOR monitor = MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST);

		UINT dpi = GetDpiForWindow(window);

		MONITORINFO monitorInfo;
		monitorInfo.cbSize = sizeof(MONITORINFO);
		GetMonitorInfo(monitor, &monitorInfo);
		RECT* monitorRect = &monitorInfo.rcMonitor;

		long width = monitorRect->right - monitorRect->left;
		long height = monitorRect->bottom - monitorRect->top;

		screen
			<< "{ "
			<< "\"width\": " << width << ", "
			<< "\"height\": " << height << ", "
			<< "\"dpi\": " << dpi
			<< " }";

		return _strdup(screen.str().c_str());
	}

	int GetMaximized() {
		return IsZoomed(window) ? 1 : 0;
	}

	int GetFullScreen() {
		return GetWindowLong(window, GWL_STYLE) & WS_OVERLAPPEDWINDOW ? 0 : 1;
	}

	void Maximize() {
		SendMessage(window, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
	}

	void Restore() {
		SendMessage(window, WM_SYSCOMMAND, SC_RESTORE, 0);
	}

	void EnterFullScreen() {
		WINDOWPLACEMENT windowPlacement = { sizeof(windowPlacement) };
		MONITORINFO monitorInfo = { sizeof(monitorInfo) };

		if (GetWindowPlacement(window, &windowPlacement) &&
			GetMonitorInfo(MonitorFromWindow(window, MONITOR_DEFAULTTOPRIMARY), &monitorInfo)) {

			DWORD windowStyle = GetWindowLong(window, GWL_STYLE);

			SetWindowLong(window, GWL_STYLE, windowStyle & ~WS_OVERLAPPEDWINDOW);
			SetWindowPos(
				window,
				HWND_TOP,
				monitorInfo.rcMonitor.left,
				monitorInfo.rcMonitor.top,
				monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
				monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
				SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
		}
	}

	void ExitFullScreen() {
		WINDOWPLACEMENT windowPlacement = { sizeof(windowPlacement) };

		DWORD windowStyle = GetWindowLong(window, GWL_STYLE);

		SetWindowLong(window, GWL_STYLE, windowStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(window, &windowPlacement);
		SetWindowPos(window, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
	}

	void FixBackground(char *color) {
		HWND textArea = FindWindowEx(window, NULL, L"VimTextArea", L"Vim text area");

		if (textArea == NULL) {
			return;
		}

		RECT rect;
		GetWindowRect(textArea, &rect);
		int margin = 100;
		rect.left -= margin;
		rect.top -= margin;
		rect.right += margin;
		rect.bottom += margin;

		SetWindowPos(textArea, HWND_TOP, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_SHOWWINDOW);

		int r, g, b;
		sscanf_s(color, "%02x%02x%02x", &r, &g, &b);
		HBRUSH brush = CreateSolidBrush(RGB(r, g, b));

		HDC dc = GetDC(textArea);
		FillRect(dc, &rect, brush);

		DeleteObject((HGDIOBJ)(HBRUSH)(brush));
		ReleaseDC(textArea, dc);
	}
};

extern "C" {
	__declspec(dllexport) char* GetScreen() {
		return (new Screen)->GetScreen();
	}

	__declspec(dllexport) int GetMaximized() {
		return (new Screen)->GetMaximized();
	}

	__declspec(dllexport) int GetFullScreen() {
		return (new Screen)->GetFullScreen();
	}

	__declspec(dllexport) void Maximize() {
		(new Screen)->Maximize();
	}

	__declspec(dllexport) void Restore() {
		(new Screen)->Restore();
	}

	__declspec(dllexport) void EnterFullScreen() {
		(new Screen)->EnterFullScreen();
	}

	__declspec(dllexport) void ExitFullScreen() {
		(new Screen)->ExitFullScreen();
	}

	__declspec(dllexport) void FixBackground(char* color) {
		(new Screen)->FixBackground(color);
	}
}
