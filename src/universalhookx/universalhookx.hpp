#pragma once

#include <Windows.h>

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

    void SetRenderCallback(void (*callback)(void));

	void CallRenderCallback();

    void SetRenderingBackend(RenderingBackend_t backend);

    void GuessRenderingBackend();

    void Hook(HWND targetWindow);

    void Unhook();

}