#include "universalhookx.hpp"

#include <iostream>

#include "imgui.h"

#include "hooks/hooks.hpp"
#include "utils.hpp"

namespace UniversalHookX {

    static void (*g_pRenderCallback)(void) = nullptr;
    static void (*g_pInitializeContextCallback)(HWND) = nullptr;

    void SetInitializeContextCallback(void (*callback)(HWND targetWindow)) {
        g_pInitializeContextCallback = callback;
    }

    void CallInitializeContextCallback(HWND targetWindow) {
        if (g_pInitializeContextCallback) {
            g_pInitializeContextCallback(targetWindow);
        }
    }

    void SetRenderCallback(void (*callback)(void)) {
        g_pRenderCallback = callback;
    }

    void CallRenderCallback() {
        if (g_pRenderCallback) {
            g_pRenderCallback();
        }
    }

    void SetRenderingBackend(RenderingBackend_t backend) {
        Utils::SetRenderingBackend(backend);
    }

    void GuessRenderingBackend() {
        Utils::SetRenderingBackend(Utils::GuessBackend());
        std::cout << "Detected rendering backend: " << Utils::RenderingBackendToStr() << std::endl;
    }

    void Hook(HWND targetWindow) {
        if (Utils::GetRenderingBackend() == RenderingBackend_t::NONE)
            GuessRenderingBackend();
        Hooks::Init(targetWindow);
    }

    void Unhook() {
        Hooks::Free();
    }

}