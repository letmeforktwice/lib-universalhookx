#include "universalhookx.hpp"

#include <iostream>
#include <string>
#include <algorithm>

#include "imgui.h"

#include "hooks/hooks.hpp"
#include "utils.hpp"

namespace UniversalHookX {

    static void (*g_pRenderCallback)(void) = nullptr;
    static void (*g_pInitializeContextCallback)(HWND) = nullptr;

    RenderingBackend_t ParseRenderingBackend(const std::string & backend) {
        // Convert to lowercase
        std::string backendLower = backend;
        std::transform(backendLower.begin(), backendLower.end(), backendLower.begin(), ::tolower);

        #define CASE(str, val) if (backendLower == str) return val;
        CASE("directx9", RenderingBackend_t::DIRECTX9)
        CASE("directx10", RenderingBackend_t::DIRECTX10)
        CASE("directx11", RenderingBackend_t::DIRECTX11)
        CASE("directx12", RenderingBackend_t::DIRECTX12)
        CASE("opengl", RenderingBackend_t::OPENGL)
        CASE("vulkan", RenderingBackend_t::VULKAN)
        #undef CASE

        return RenderingBackend_t::NONE;
    }

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