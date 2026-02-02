/*
    provided by Starclub
*/

#include "hooks.hpp"
#include "../Public/Imgui/imgui.h"
#include "../Public/Imgui/backend/imgui_impl_dx11.h"
#include "../Public/Imgui/backend/imgui_impl_win32.h"

#pragma comment(lib, "d3d11.lib")
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace {
    ID3D11Device* pDevice = nullptr;
    ID3D11DeviceContext* pContext = nullptr;
    ID3D11RenderTargetView* mainRenderTargetView = nullptr;
    bool imguiInitialized = false;
}

bool Hooks::Initialize() {
    if (MH_Initialize() != MH_OK)
        return false;

    setupDevice();
    window = FindWindowA("SDL_app", nullptr);

    return true;
}

void Hooks::Destroy() {
    MH_DisableHook(MH_ALL_HOOKS);
    MH_RemoveHook(MH_ALL_HOOKS);
    MH_Uninitialize();

    if (window && oWndProc) 
    {
        SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)oWndProc);
    }
    
    if (imguiInitialized) 
    {
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();

        if (mainRenderTargetView) mainRenderTargetView->Release();
        if (pContext) pContext->Release();
        if (pDevice) pDevice->Release();
    }
}

void Hooks::setupDevice() {
    WNDCLASSEXA wc = { sizeof(WNDCLASSEXA) };
    wc.lpfnWndProc = DefWindowProcA;
    wc.hInstance = GetModuleHandleA(nullptr);
    wc.lpszClassName = "null";

    RegisterClassExA(&wc);
    HWND hwnd = CreateWindowA(
        "null",
        "",
        WS_OVERLAPPEDWINDOW,
        0, 0, 100, 100,
        nullptr, nullptr,
        wc.hInstance, nullptr
    );

    DXGI_SWAP_CHAIN_DESC swapDesc = {};
    swapDesc.BufferCount = 1;
    swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapDesc.OutputWindow = hwnd;
    swapDesc.SampleDesc.Count = 1;
    swapDesc.Windowed = TRUE;
    swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    IDXGISwapChain* swapChain = nullptr;
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* context = nullptr;
    D3D_FEATURE_LEVEL featureLevel;

    if (SUCCEEDED
    (
        D3D11CreateDeviceAndSwapChain
        (
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &swapDesc,
        &swapChain,
        &device,
        &featureLevel,
        &context
        )
    ))
    {
        void** vtable = *reinterpret_cast<void***>(swapChain);
        void* presentAddr = vtable[8];

        if (MH_CreateHook(presentAddr, &Present, reinterpret_cast<void**>(&oPresent)) == MH_OK) {
            MH_EnableHook(presentAddr);
        }

        swapChain->Release();
        device->Release();
        context->Release();
    }

    DestroyWindow(hwnd);
    UnregisterClassA("null", wc.hInstance);
}

HRESULT __stdcall Hooks::Present(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {
    static bool init = false;

    if (!init) {
        if (FAILED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice))) {
            return oPresent(pSwapChain, SyncInterval, Flags);
        }

        pDevice->GetImmediateContext(&pContext);

        DXGI_SWAP_CHAIN_DESC sd;
        pSwapChain->GetDesc(&sd);
        window = sd.OutputWindow;

        ID3D11Texture2D* backBuffer = nullptr;
        pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
        pDevice->CreateRenderTargetView(backBuffer, nullptr, &mainRenderTargetView);
        backBuffer->Release();

        oWndProc = (WndProcFn)GetWindowLongPtr(window, GWLP_WNDPROC);
        SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);

        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.IniFilename = nullptr;

        ImGui_ImplWin32_Init(window);
        ImGui_ImplDX11_Init(pDevice, pContext);

        init = true;
        imguiInitialized = true;
    }

    if (GetAsyncKeyState(VK_INSERT) & 1) {
        menuOpen = !menuOpen;
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (menuOpen) {
        ImGui::Begin("Menu", &menuOpen);
        ImGui::Text("text");
        ImGui::End();
    }

    ImGui::Render();
    pContext->OMSetRenderTargets(1, &mainRenderTargetView, nullptr);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    return oPresent(pSwapChain, SyncInterval, Flags);
}

LRESULT __stdcall Hooks::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (menuOpen && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)) {
        return true;
    }
    return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}