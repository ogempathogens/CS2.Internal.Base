/*
    provided by Starclub
*/

#include <vector>
#include <WTypesbase.h>
#include <memory>
#include <Psapi.h>
#include "../common.hpp"

uintptr_t getModuleBase(const char* module)
{
    return reinterpret_cast<uintptr_t>(GetModuleHandleA(module));
}

uintptr_t scanSig(const char* module, const char* sig)
{
    MODULEINFO modInfo = { 0 };
    HMODULE hModule = GetModuleHandleA(module);

    std::vector<std::pair<uint8_t, bool>> buf;
    const char* pat = sig;
    // tbf
}


