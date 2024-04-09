#pragma once

#include "universalhookx.hpp"

namespace UniversalHookX::Utils {
	void SetRenderingBackend(RenderingBackend_t eRenderingBackend);
	RenderingBackend_t GetRenderingBackend( );
	const char* RenderingBackendToStr( );
	
	int GetCorrectDXGIFormat(int eCurrentFormat);

	RenderingBackend_t GuessBackend();
}

namespace U = UniversalHookX::Utils;
