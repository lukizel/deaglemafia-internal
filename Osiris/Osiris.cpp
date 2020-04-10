#include "Config.h"
#include "GUI.h"
#include "Hooks.h"
#include "Interfaces.h"
#include "Memory.h"
#include "Netvars.h"
#include <iostream>
#include <lmcons.h>

static HMODULE module;
static WNDPROC originalWndproc;

static LRESULT WINAPI init(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
    SetWindowLongPtr(FindWindowW(L"Valve001", nullptr), GWLP_WNDPROC, LONG_PTR(originalWndproc));
    config = std::make_unique<Config>("Osiris");
    gui = std::make_unique<GUI>();
    interfaces = std::make_unique<const Interfaces>();
    memory = std::make_unique<Memory>();
    netvars = std::make_unique<Netvars>();
    hooks = std::make_unique<Hooks>(module);
    char username[UNLEN + 1];
    DWORD username_len = UNLEN + 1;
    GetUserName(username, &username_len);
    AllocConsole();
    freopen_s((FILE**)stdin, "CONIN$", "r", (FILE*)stdin);
    freopen_s((FILE**)stdout, "CONOUT$", "w", (FILE*)stdout);
    freopen_s((FILE**)stderr, "CONERR$", "w", (FILE*)stderr);
    std::cout << "[INFO] Initialized succesfully" << "\n";
    std::cout << "[INFO] Welcome to the [deaglemafia] internal project, " << username << "." << "\n";

    return CallWindowProc(originalWndproc, window, msg, wParam, lParam);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        module = hModule;
        originalWndproc = WNDPROC(SetWindowLongPtr(FindWindowW(L"Valve001", nullptr), GWLP_WNDPROC, LONG_PTR(init)));
    }
    return TRUE;
}
