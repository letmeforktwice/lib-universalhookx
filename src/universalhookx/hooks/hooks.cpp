#include <cstdio>
#include <mutex>
#include <thread>

#include "hooks.hpp"

#include "backend/dx10/hook_directx10.hpp"
#include "backend/dx11/hook_directx11.hpp"
#include "backend/dx12/hook_directx12.hpp"
#include "backend/dx9/hook_directx9.hpp"

#include "backend/opengl/hook_opengl.hpp"
#include "backend/vulkan/hook_vulkan.hpp"

#include "../console/console.hpp"
#include "../menu/menu.hpp"
#include "../utils/utils.hpp"

#include "input/hook_mouse.hpp"
#include "input/hook_dinput8.hpp"

#include "MinHook.h"

static HWND g_hWindow = NULL;
static std::mutex g_mReinitHooksGuard;

static DWORD WINAPI ReinitializeGraphicalHooks(LPVOID lpParam) {
    LOG("[!] ReinitializeGraphicalHooks is disabled!\n");
    
    // std::lock_guard<std::mutex> guard{g_mReinitHooksGuard};

    // LOG("[!] Hooks will reinitialize!\n");

    // HWND hNewWindow = U::GetProcessWindow( );
    // while (hNewWindow == reinterpret_cast<HWND>(lpParam)) {
    //     hNewWindow = U::GetProcessWindow( );
    // }

    // H::bShuttingDown = true;

    // H::Free( );
    // H::Init(hNewWindow);

    // H::bShuttingDown = false;
    // Menu::bShowMenu = true;

    return 0;
}

int _getCursorDisplayCount() {
    auto count = ShowCursor(true) - 1;
    ShowCursor(false);
    return count;
}
void _setCursorDisplayCount(int count) {
    auto current = _getCursorDisplayCount();
    if (current < count) {
        while (ShowCursor(true) < count);
    } else if (current > count) {
        while (ShowCursor(false) > count);
    }
}


static WNDPROC oWndProc;
static LRESULT WINAPI WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    Mouse::enableApis = true;

    static int gameCursorDisplayCount = 0;

    if (uMsg == WM_KEYDOWN) {
        if (wParam == VK_INSERT) {
            Menu::bShowMenu = !Menu::bShowMenu;
            if (Menu::bShowMenu) {
                gameCursorDisplayCount = _getCursorDisplayCount();
                _setCursorDisplayCount(0);

                // Unfocus window and then refocus to fix input issues
                SetForegroundWindow(NULL);
                SetForegroundWindow(hWnd);
                SetFocus(hWnd);
            } else {
                _setCursorDisplayCount(gameCursorDisplayCount);
            }
            return 0;
        } else if (wParam == VK_HOME) {
            LOG("[!] ReinitializeGraphicalHooks is disabled!\n");
            // HANDLE hHandle = CreateThread(NULL, 0, ReinitializeGraphicalHooks, NULL, 0, NULL);
            // if (hHandle != NULL)
            //     CloseHandle(hHandle);
            // return 0;
        }
    } else if (uMsg == WM_DESTROY) {
        LOG("[!] ReinitializeGraphicalHooks is disabled!\n");
        // HANDLE hHandle = CreateThread(NULL, 0, ReinitializeGraphicalHooks, hWnd, 0, NULL);
        // if (hHandle != NULL)
        //     CloseHandle(hHandle);
    }

    LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    if (Menu::bShowMenu) {
        ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);

        // (Doesn't work for some games like 'Sid Meier's Civilization VI')
        // Window may not maximize from taskbar because 'H::bShowDemoWindow' is set to true by default. ('hooks.hpp')
        //
        // return ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam) == 0;
    }

    Mouse::enableApis = false;

    return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

namespace Hooks {
    void Init(HWND targetWindow) {
        // g_hWindow = U::GetProcessWindow( );
        g_hWindow = targetWindow;

#ifdef DISABLE_LOGGING_CONSOLE
        bool bNoConsole = GetConsoleWindow( ) == NULL;
        if (bNoConsole) {
            AllocConsole( );
        }
#endif

        RenderingBackend_t eRenderingBackend = U::GetRenderingBackend( );
        switch (eRenderingBackend) {
            case DIRECTX9:
                DX9::Hook(g_hWindow);
                break;
            case DIRECTX10:
                DX10::Hook(g_hWindow);
                break;
            case DIRECTX11:
                DX11::Hook(g_hWindow);
                break;
            case DIRECTX12:
                DX12::Hook(g_hWindow);
                break;
            case OPENGL:
                GL::Hook(g_hWindow);
                break;
            case VULKAN:
                VK::Hook(g_hWindow);
                break;
        }

#ifdef DISABLE_LOGGING_CONSOLE
        if (bNoConsole) {
            FreeConsole( );
        }
#endif

        oWndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(g_hWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProc)));

        Mouse::Hook( );
        DI8::Hook( );
    }

    void Free( ) {
        if (oWndProc) {
            SetWindowLongPtr(g_hWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(oWndProc));
        }

        MH_DisableHook(MH_ALL_HOOKS);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        RenderingBackend_t eRenderingBackend = U::GetRenderingBackend( );
        switch (eRenderingBackend) {
            case DIRECTX9:
                DX9::Unhook( );
                break;
            case DIRECTX10:
                DX10::Unhook( );
                break;
            case DIRECTX11:
                DX11::Unhook( );
                break;
            case DIRECTX12:
                DX12::Unhook( );
                break;
            case OPENGL:
                GL::Unhook( );
                break;
            case VULKAN:
                VK::Unhook( );
                break;
        }
    }
} // namespace Hooks
