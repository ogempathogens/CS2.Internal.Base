/*
    provided by Starclub
*/

#pragma once

#include "../Source/common.hpp"
#include "../Public/Minhook/MinHook.h"

class Hooks {
public:
    static bool Initialize();
    static void Destroy();
    static void setupDevice();

    using PresentFn = HRESULT(__stdcall*)(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
    using WndProcFn = LRESULT(__stdcall*)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    using CreateDeviceFn = HRESULT(__stdcall*)(IDXGIAdapter*, D3D_DRIVER_TYPE, HMODULE, UINT, const D3D_FEATURE_LEVEL*, UINT, UINT, const DXGI_SWAP_CHAIN_DESC*, IDXGISwapChain**, ID3D11Device**, D3D_FEATURE_LEVEL*, ID3D11DeviceContext**);

    static HRESULT __stdcall Present(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
    static LRESULT __stdcall WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    static inline PresentFn oPresent = nullptr;
    static inline WndProcFn oWndProc = nullptr;
    static inline CreateDeviceFn oCreateDevice = nullptr;
    static inline HWND window = nullptr;
    static inline bool hooksReady = false;
    static inline bool menuOpen = true;
};