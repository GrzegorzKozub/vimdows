#include <sstream>
#include <string>
#include <windows.h>
#include <cstdint>

using namespace std;

class Screen {
private:

    HWND window = NULL;
    HWND textArea = NULL;

    static BOOL CALLBACK EnumThreadWndProc(HWND hwnd, LPARAM lParam) {
        HWND* hwndPtr = (HWND*)lParam;
        if (GetParent(hwnd)) {
            *hwndPtr = NULL;
            return true;
        } else {
            *hwndPtr = hwnd;
            return false;
        }
    }

public:

    Screen() {
        EnumThreadWindows(GetCurrentThreadId(), EnumThreadWndProc, (LPARAM)&window);
        textArea = FindWindowEx(window, NULL, "VimTextArea", "Vim text area");
    }

    char* GetScreen() {
        std::stringstream screen;
        HDC dc = GetDC(NULL);

        screen
            << "{ "
            << "'width': " << GetDeviceCaps(dc, HORZRES) << ", "
            << "'height': " << GetDeviceCaps(dc, VERTRES) << ", "
            << "'dpi': " << GetDeviceCaps(dc, LOGPIXELSX)
            << " }";

        ReleaseDC(NULL, dc);

        return strdup(screen.str().c_str());
    }

    int GetMaximized() {
        return IsZoomed(window) ? 1 : 0;
    }

    int GetFullScreen() {
        return GetWindowLong(window, GWL_STYLE) & WS_CAPTION ? 0 : 1;
    }

    void Maximize() {
        SendMessage(window, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
    }

    void EnterFullScreen() {
        RECT rect;
        GetWindowRect(textArea, &rect);

        HMONITOR monitor = MonitorFromRect(&rect, MONITOR_DEFAULTTONEAREST);
        MONITORINFO monitorInfo;
        monitorInfo.cbSize = sizeof(monitorInfo);
        GetMonitorInfo(monitor, &monitorInfo);

        int x = monitorInfo.rcMonitor.left;
        int y = monitorInfo.rcMonitor.top;
        int dx = monitorInfo.rcMonitor.right - x;
        int dy = monitorInfo.rcMonitor.bottom - y;

        SetWindowLong(window, GWL_STYLE, GetWindowLong(window, GWL_EXSTYLE) & ~WS_BORDER);
        SetWindowLong(window, GWL_STYLE, GetWindowLong(window, GWL_STYLE) & ~WS_CAPTION);
        SetWindowLong(window, GWL_EXSTYLE, GetWindowLong(window, GWL_STYLE) & ~WS_EX_CLIENTEDGE);
        SetWindowLong(window, GWL_EXSTYLE, GetWindowLong(window, GWL_STYLE) & ~WS_EX_WINDOWEDGE);
        SetWindowPos(window, HWND_TOP, x, y, dx, dy, SWP_SHOWWINDOW);

        SetWindowLong(textArea, GWL_EXSTYLE, GetWindowLong(textArea, GWL_STYLE) & ~WS_EX_WINDOWEDGE);
        SetWindowPos(textArea, HWND_TOP, 0, 0, dx, dy, SWP_SHOWWINDOW);
    }

    void ExitFullScreen() {
        SetWindowLong(window, GWL_EXSTYLE, GetWindowLong(window, GWL_EXSTYLE) | WS_BORDER);
        SetWindowLong(window, GWL_STYLE, GetWindowLong(window, GWL_STYLE) | WS_CAPTION);
        SetWindowLong(window, GWL_STYLE, GetWindowLong(window, GWL_STYLE) | WS_SYSMENU);
        SetWindowLong(window, GWL_STYLE, GetWindowLong(window, GWL_STYLE) | WS_MINIMIZEBOX);
        SetWindowLong(window, GWL_STYLE, GetWindowLong(window, GWL_STYLE) | WS_MAXIMIZEBOX);
        SetWindowLong(window, GWL_STYLE, GetWindowLong(window, GWL_STYLE) | WS_SYSMENU);
        SetWindowLong(window, GWL_STYLE, GetWindowLong(window, GWL_STYLE) | WS_EX_CLIENTEDGE);
        SetWindowLong(window, GWL_EXSTYLE, GetWindowLong(window, GWL_EXSTYLE) | WS_EX_WINDOWEDGE);
        SetWindowLong(window, GWL_STYLE, GetWindowLong(window, GWL_STYLE) | WS_THICKFRAME);
        SetWindowLong(window, GWL_STYLE, GetWindowLong(window, GWL_STYLE) | WS_DLGFRAME);
    }

    void FixBackground(char *color) {
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
        sscanf(color, "%02x%02x%02x", &r, &g, &b);
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

