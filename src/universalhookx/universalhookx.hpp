#pragma once

#include <Windows.h>
#include <string>

namespace UniversalHookX {

	enum RenderingBackend_t {
		NONE = 0,

		DIRECTX9,
		DIRECTX10,
		DIRECTX11,
		DIRECTX12,

		OPENGL,
		VULKAN,
	};

	RenderingBackend_t ParseRenderingBackend(const std::string& backend);

	void SetInitializeContextCallback(void (*callback)(HWND targetWindow));

	void CallInitializeContextCallback(HWND targetWindow);

    void SetRenderCallback(void (*callback)(void));

	void CallRenderCallback();

    void SetRenderingBackend(RenderingBackend_t backend);

	RenderingBackend_t GetRenderingBackend();

	std::string GetRenderingBackendName();

    void GuessRenderingBackend();

    void Hook(HWND targetWindow);

    void Unhook();

}