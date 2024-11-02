#pragma once
#include <d3d9.h>
#include <stdexcept>
#include <intrin.h>
#include <Windows.h>
#include <thread>
#include <cstdint>

#include "../minhook/minhook.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_win32.h"
#include "../imgui/imgui_impl_dx9.h"

namespace ui
{
    inline bool open = true;
    inline bool setup = false;
    inline HWND window = nullptr;
    inline WNDCLASSEX windowClass = {};
    inline WNDPROC originalWindowProcess = nullptr;
    inline LPDIRECT3DDEVICE9 device = nullptr;
    inline LPDIRECT3D9 d3d9 = nullptr;
    extern const char* windowName;

    bool InitializeWindowClass(const char* windowClassName) noexcept;
    void DestroyWindowClass() noexcept;
    bool InitializeWindow(const char* windowClassName) noexcept;
    void DestroyWindow() noexcept;
    bool InitializeDirectX() noexcept;
    void DestroyDirectX() noexcept;
    void Initialize();
    void InitializeMenu(LPDIRECT3DDEVICE9 device) noexcept;
    void Destroy() noexcept;
    void Render() noexcept;
}

namespace hooks
{
    void Initialize();
    void CleanUP();

    constexpr void* VirtualFunction(void* thisptr, size_t index) noexcept
    {
        return (*static_cast<void***>(thisptr))[index];
    }

    using EndSceneFn = long(__thiscall*)(void*, IDirect3DDevice9*) noexcept;
    inline EndSceneFn EndSceneOriginal = nullptr;
    long __stdcall EndScene(IDirect3DDevice9* device) noexcept;

    using ResetFn = HRESULT(__thiscall*)(void*, IDirect3DDevice9*, D3DPRESENT_PARAMETERS*) noexcept;
    inline ResetFn ResetOriginal = nullptr;
    HRESULT __stdcall Reset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* params) noexcept;
}
