/*
    provided by Starclub
*/

#include <vector>
#include <WTypesbase.h>
#include <memory>
#include <Psapi.h>
#include <sstream>
#include <string>
#include "../common.hpp"

uintptr_t scanSig(const char* module, const char* signature)
{
    HMODULE mod = GetModuleHandleA(module);
    if (!mod)
        return 0;

    MODULEINFO modInfo;
    if (!GetModuleInformation(GetCurrentProcess(), mod, &modInfo, sizeof(MODULEINFO)))
        return 0;

    uintptr_t modBase = (uintptr_t)modInfo.lpBaseOfDll;
    uintptr_t modSize = (uintptr_t)modInfo.SizeOfImage;

    std::vector<uint8_t> sigBytes;
    std::string mask;
    std::istringstream stream(signature);
    std::string token;

    while (stream >> token)
    {
        if (token == "?" || token == "??")
        {
            sigBytes.push_back(0x00);
            mask += '?';
        }
        else
        {
            sigBytes.push_back(static_cast<uint8_t>(std::stoi(token, nullptr, 16)));
            mask += 'x';
        }
    }

    size_t sigSize = sigBytes.size();

    if (sigSize > modSize)
        return 0;

    for (size_t i = 0; i < modSize - sigSize; i++)
    {
        uintptr_t currAddr = modBase + i;
        bool found = true;

        for (size_t j = 0; j < sigSize; j++)
        {
            if (mask[j] == '?')
            {
                continue;
            }
            else
            {
                uint8_t memByte = *reinterpret_cast<uint8_t*>(currAddr + j);
                if (memByte != sigBytes[j])
                {
                    found = false;
                    break;
                }
            }
        }

        if (found)
        {
            return currAddr;
        }
    }

    return 0;
}