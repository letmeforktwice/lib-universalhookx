#pragma once
#include <Windows.h>

namespace UniversalHookX::Hooks {

	void InitializeContext(HWND targetWindow);

	void Init(HWND targetWindow);
	void Free( );

	inline bool bShuttingDown;
}

namespace H = UniversalHookX::Hooks;
