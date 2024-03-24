#include <Windows.h>
#include <Psapi.h>
#include <thread>
#include <string>
#include <filesystem>
#include <dxgi.h>

#include "utils.hpp"

#include "console.hpp"

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

static UniversalHookX::RenderingBackend_t g_eRenderingBackend = UniversalHookX::NONE;

static BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam) {
	const auto isMainWindow = [ handle ]( ) {
		return GetWindow(handle, GW_OWNER) == nullptr && IsWindowVisible(handle);
	};

	DWORD pID = 0;
	GetWindowThreadProcessId(handle, &pID);

	if (GetCurrentProcessId( ) != pID || !isMainWindow( ) || handle == GetConsoleWindow( ))
		return TRUE;

	*reinterpret_cast<HWND*>(lParam) = handle;

	return FALSE;
}

namespace UniversalHookX::Utils {
	
	HMODULE GetCurrentImageBase( ) {
		return (HINSTANCE)(&__ImageBase);
	}

	void SetRenderingBackend(RenderingBackend_t eRenderingBackground) {
		g_eRenderingBackend = eRenderingBackground;
	}

	RenderingBackend_t GetRenderingBackend( ) {
		return g_eRenderingBackend;
	}

	const char* RenderingBackendToStr( ) {
		RenderingBackend_t eRenderingBackend = GetRenderingBackend( );

		switch (eRenderingBackend) {
			#define RB2STR(x) case x: return #x
			RB2STR(DIRECTX9);
			RB2STR(DIRECTX10);
			RB2STR(DIRECTX11);
			RB2STR(DIRECTX12);
			RB2STR(OPENGL);
			RB2STR(VULKAN);
			#undef RB2STR
		}

		return "NONE/UNKNOWN";
	}

	int GetCorrectDXGIFormat(int eCurrentFormat) {
		switch (eCurrentFormat) {
			case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB: return DXGI_FORMAT_R8G8B8A8_UNORM;
		}

		return eCurrentFormat;
	}

	RenderingBackend_t GuessBackend() {
		char currentImagePathC[MAX_PATH];
		GetModuleFileNameA(GetCurrentImageBase(), currentImagePathC, MAX_PATH);
		std::filesystem::path currentImagePath(currentImagePathC);
		std::string currentImageName = currentImagePath.filename().string();

		// LOG("[+] Current image name: %s\n", currentImageName.c_str());

		HMODULE modules[1024];
		DWORD needed;
		if (!EnumProcessModules(GetCurrentProcess(), modules, sizeof(modules), &needed)) {
			LOG("[-] Failed to enumerate process modules.\n");
			return NONE;
		}

		struct BackendInfo {
			RenderingBackend_t backend;
			std::string moduleName;
		};

		BackendInfo backendInfos[] = {
			{ DIRECTX12, "d3d12.dll" }, // Max priority
			{ DIRECTX10, "d3d10.dll" },
			{ VULKAN, "vulkan-1.dll" },
			{ DIRECTX11, "d3d11.dll" },
			{ OPENGL, "opengl32.dll" },
			{ DIRECTX9, "d3d9.dll" },   // Min priority
		};

		// Check each backend from highest to lowest priority.
		for (auto& backendInfo : backendInfos) {
			for (size_t i = 0; i < (needed / sizeof(HMODULE)); i++) {
				char moduleNameC[MAX_PATH];
				if (GetModuleFileNameExA(GetCurrentProcess(), modules[i], moduleNameC, MAX_PATH)) {
					std::string moduleName(moduleNameC);

					// Everything after this was loaded by us (or some other dll injected after us).
					if (moduleName.find(currentImageName) != std::string::npos)
						break;

					if (moduleName.find(backendInfo.moduleName) != std::string::npos) {
						// LOG("[+] Detected %s backend.\n", backendInfo.moduleName.c_str());
						return backendInfo.backend;
					}
				}
			}
		}

		return NONE;
	}
}
