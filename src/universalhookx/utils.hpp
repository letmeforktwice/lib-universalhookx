#pragma once

#include "universalhookx.hpp"

// enum RenderingBackend_t {
// 	NONE = 0,

// 	DIRECTX9,
// 	DIRECTX10,
// 	DIRECTX11,
// 	DIRECTX12,

// 	OPENGL,
// 	VULKAN,
// };

namespace UniversalHookX::Utils {
	void SetRenderingBackend(RenderingBackend_t eRenderingBackend);
	RenderingBackend_t GetRenderingBackend( );
	const char* RenderingBackendToStr( );
	
	int GetCorrectDXGIFormat(int eCurrentFormat);

	RenderingBackend_t GuessBackend();
}

namespace U = UniversalHookX::Utils;
