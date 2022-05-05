#include <Windows.h>
#include <win32/resources.hh>

#include <ui.hh>
#include <cfg.hh>

#include <string>
#include <thread>

#define MODLOADER_WINDOW_CLASS "modloader_window"
#define WM_TRAY (WM_USER + 0x100)

namespace modloader::ui
{
    HINSTANCE instance_handle = nullptr;
    HWND window_handle = nullptr;
    HMENU menu_handle = nullptr;

    NOTIFYICONDATAA tray;
    std::thread gui_thread;

    LRESULT CALLBACK shellicon_wndproc(HWND window_handle, UINT msg, WPARAM w, LPARAM l)
    {
        switch (msg)
        {
        case WM_TRAY:
            switch (l)
            {
            case WM_RBUTTONDOWN:
            case WM_CONTEXTMENU:
                POINT cursor_pos;
                GetCursorPos(&cursor_pos);
                SetForegroundWindow(window_handle);
                TrackPopupMenu(menu_handle, 0, cursor_pos.x, cursor_pos.y, 0, window_handle, nullptr);
                break;

            default:
                break;
            }
            break;

        case WM_COMMAND:
            switch (LOWORD(w))
            {
            case 0:                             // Exit
                DestroyWindow(window_handle);
                break;

            case 1:                             // Reload config
                cfg::load();
                break;

            default:
                break;
            }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            break;
        }
        return DefWindowProcA(window_handle, msg, w, l);
    }

    MSG msg;
    void enter_loop()
    {
        do
        {
            while (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);

                if (msg.message == WM_QUIT) break;
            }
        }
        while (msg.message != WM_QUIT);
    }

    void raw_log_string(const char* string)
    {
        MSGBOXPARAMSA msg;
        std::memset(&msg, 0, sizeof(msg));
        msg.cbSize = sizeof(msg);
        msg.dwStyle = MB_USERICON;
        msg.hInstance = instance_handle;
        msg.hwndOwner = window_handle;
        msg.lpszIcon = MAKEINTRESOURCEA(IDI_MAIN_ICON);
        msg.lpszCaption = "modloader";
        msg.lpszText = string;
        MessageBoxIndirectA(&msg);
    }

    void init()
    {
        instance_handle = GetModuleHandleA(nullptr);

        // Load our icon
        auto icon_handle = LoadIconA(GetModuleHandleA(nullptr), MAKEINTRESOURCEA(IDI_MAIN_ICON));

        // Create a window class for the trayicon and message boxes
        WNDCLASSA window_class;
        std::memset(&window_class, 0, sizeof(window_class));
        window_class.hIcon = icon_handle;
        window_class.hInstance = instance_handle;
        window_class.lpfnWndProc = shellicon_wndproc;
        window_class.lpszClassName = MODLOADER_WINDOW_CLASS;

        auto class_atom = RegisterClassA(&window_class);
        if (class_atom == 0)
        {
            raw_log_fmt("Failed to register window class: %x\n", GetLastError());
            return;
        }

        // Create an invisible window using the class
        window_handle = CreateWindowExA(0, MODLOADER_WINDOW_CLASS, "ModLoader", 0, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, nullptr, nullptr, instance_handle, nullptr);

        // Create tray context menu
        menu_handle = CreatePopupMenu();
        AppendMenuA(menu_handle, MF_STRING, 0, "Exit");
        AppendMenuA(menu_handle, MF_STRING, 1, "Reload config");

        // Create trayicon
        std::memset(&tray, 0, sizeof(tray));
        tray.cbSize = sizeof(tray);
        tray.uVersion = NOTIFYICON_VERSION_4;
        tray.hIcon = icon_handle;
        tray.hWnd = window_handle;
        tray.uID = 1337;
        tray.uCallbackMessage = WM_TRAY;
        tray.dwInfoFlags = NIIF_INFO;
        tray.uTimeout = 10000;
        tray.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
        std::strcpy(tray.szTip, "ModLoader");
        Shell_NotifyIconA(NIM_ADD, &tray);
    }

    void exit()
    {
        DestroyMenu(menu_handle);
        Shell_NotifyIconA(NIM_DELETE, &tray);
        CloseWindow(window_handle);
    }
}
