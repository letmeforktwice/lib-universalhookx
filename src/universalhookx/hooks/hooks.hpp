#pragma once
#include <Windows.h>

namespace Hooks {
	void Init(HWND targetWindow);
	void Free( );

	inline bool bShuttingDown;
}

namespace H = Hooks;
