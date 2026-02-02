/*
    provided by Starclub
*/

#include "../Source/common.hpp"
#include "../Source/hooks.hpp"

//#define _dbg

void Init(HMODULE hModule) {
#ifdef _dbg
    AllocConsole();
    freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
#endif

    if (!Hooks::Initialize()) {
        std::cout << "failed on Hooks::Initialize\n";
        FreeLibraryAndExitThread(hModule, 0);
        return;
    }
    std::cout << "Hooks::Initialize success";

    while (true) {
        if (GetAsyncKeyState(VK_END) & 0x8000) {
            break;
        }
        Sleep(10);
    }

    Hooks::Destroy();

#ifdef _dbg
    fclose(stdout);
    FreeConsole();
#endif

    FreeLibraryAndExitThread(hModule, 0);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);

        HANDLE hThread = CreateThread(
            nullptr,
            0,
            reinterpret_cast<LPTHREAD_START_ROUTINE>(Init),
            hModule,
            0,
            nullptr
        );

        if (hThread) {
            CloseHandle(hThread);
        }
    }

    return TRUE;
}