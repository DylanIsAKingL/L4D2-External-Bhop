#include <iostream>
#include <Windows.h>
#include <cstdint>
#include <vector>
#include <TlHelp32.h>
#include <tchar.h>
#include <stdlib.h>
#include <string>
#include <thread>
#include "offset.h"

namespace game
{
    HWND window;
    DWORD procId;

    HANDLE process;

    const int five = 5;
    const int four = 4;

    uintptr_t client;
}

uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName)
{
    uintptr_t modBaseAddr = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
    if (hSnap != INVALID_HANDLE_VALUE)
    {
        MODULEENTRY32 modEntry;
        modEntry.dwSize = sizeof(modEntry);
        if (Module32First(hSnap, &modEntry))
        {
            do
            {
                if (!_wcsicmp(modEntry.szModule, modName))
                {
                    modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
                    break;
                }
            } while (Module32Next(hSnap, &modEntry));
        }
    }
    CloseHandle(hSnap);
    return modBaseAddr;
}

void bhop()
{
    char name[] = "Left 4 Dead 2 - Direct3D 9";
    game::window = FindWindowA(NULL, name); // Getting window

    if (!game::window) // if window doesnt exist it will ask for the user to input the window name manually
    {
        std::cout << "L4D2 Window not found!\n";
        std::cout << "Enter L4D2 window name manually: ";
        std::cin >> name;
        game::window = FindWindowA(NULL, name);

        if (!game::window) // if its still invalid it will close the window
        {
            std::cout << "Window not found, closing in 3 seconds\n\n";
            std::this_thread::sleep_for(std::chrono::seconds(3));
            return;
        }
    }

    GetWindowThreadProcessId(game::window, &game::procId); // Getting process id

    game::process = OpenProcess(PROCESS_ALL_ACCESS, NULL, game::procId);

    game::client = GetModuleBaseAddress(game::procId, L"client.dll");

    if (!game::client) // checking whether client.dll is a in the game
    {
        std::cout << "Client.dll not found, Closing in 3 seconds\n\n";
        std::this_thread::sleep_for(std::chrono::seconds(3));
        return;
    }

    bool bhopEnabled = true;
    bool wasBhopping = false;
    bool wasHoldingInsert = false;
    const int four = 4;
    const int five = 5;

    std::cout << "   __       __                        __     \n  / \     /  |                      /  |\n  $$  \ / $$ | __    __   _______  _$$ | _\n  $$$  \ / $$$ |/  |  /  | /       |/ $$ |\n  $$$$ / $$$$ | $$ | $$ |/$$$$$$$ / $$$$$$ /\n  $$ $$ $$ / $$ | $$ | $$ | $$      \   $$ | __\n  $$ | $$$ / $$ | $$ \__$$ | $$$$$$ | $$ |/  |\n  $$ | $ / $$ | $$    $$ |/     $$ / $$  $$ /\n  $$ / $$ / $$$$$$$ | $$$$$$$ / $$$$/\n               /  \__$$ |\n               $$    $$ /\n                $$$$$$ / \n\n";

    std::cout << " Dm https://discordapp.com/users/717369125965398027 if you find any bugs or it doesnt work\n\n";

    std::cout << " Press insert to enable / disable bhop\n\n";
    std::cout << " Bhop Enabled [Y/N]: Y";

    while (true)
    {
        // Disabling / Enabling Bhop
        bool isHoldingInsert = GetAsyncKeyState(VK_INSERT);
        if (GetAsyncKeyState(VK_INSERT) && !wasHoldingInsert)
        {
            bhopEnabled = !bhopEnabled;

            system("CLS");

            std::cout << "   __       __                        __     \n  / \     /  |                      /  |\n  $$  \ / $$ | __    __   _______  _$$ | _\n  $$$  \ / $$$ |/  |  /  | /       |/ $$ |\n  $$$$ / $$$$ | $$ | $$ |/$$$$$$$ / $$$$$$ /\n  $$ $$ $$ / $$ | $$ | $$ | $$      \   $$ | __\n  $$ | $$$ / $$ | $$ \__$$ | $$$$$$ | $$ |/  |\n  $$ | $ / $$ | $$    $$ |/     $$ / $$  $$ /\n  $$ / $$ / $$$$$$$ | $$$$$$$ / $$$$/\n               /  \__$$ |\n               $$    $$ /\n                $$$$$$ / \n\n";

            std::cout << " Dm https://discordapp.com/users/717369125965398027 if you find any bugs or it doesnt work\n\n";

            std::cout << " Press insert to enable / disable bhop\n\n";

            if (bhopEnabled)
                std::cout << " Bhop Enabled [Y/N]: Y";
            else
                std::cout << " Bhop Enabled [Y/N]: N";
        }

        wasHoldingInsert = isHoldingInsert;

        if (!bhopEnabled)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(3)); // sleeping thread
            continue;
        }

        if (!FindWindowA(NULL, name)) // checking whether window exists every frame
        {
            std::cout << "\nL4D2 Closed, closing in 3 seconds";
            std::this_thread::sleep_for(std::chrono::seconds(3));
            return;
        }

        // Variables

        std::uintptr_t dwLocalPlayer;
        ReadProcessMemory(game::process, (LPCVOID)(game::client + offset::dwLocalPlayer), &dwLocalPlayer, sizeof(std::uintptr_t), nullptr); // Getting local player

        if (wasBhopping) // resetting dwForceJump state if player was bhopping last frame
            WriteProcessMemory(game::process, (LPVOID*)(game::client + offset::dwForceJump), &four, 4, 0);

        if (GetAsyncKeyState(VK_SPACE)) // checking whether player is holding spacebar
        {
            int dwfFlags;
            ReadProcessMemory(game::process, (LPCVOID)(dwLocalPlayer + offset::dwfFlags), &dwfFlags, 4, nullptr);

            if (dwfFlags == 129 || dwfFlags == 641 || dwfFlags == 131 || dwfFlags == 643) // checking character state (129 = Ground, 641 = Water, 131 = groundCrouching & 643 = waterCrouching)
            {
                WriteProcessMemory(game::process, (LPVOID*)(game::client + offset::dwForceJump), &five, sizeof(const int), 0); // writing dwForceJump forcing player to jump
                wasBhopping = true;
            }
            else
                wasBhopping = false;
        }
        else
            wasBhopping = false;

        std::this_thread::sleep_for(std::chrono::milliseconds(3)); // sleeping thread
    }
}

int main()
{
    SetConsoleTitle(L"L4D2 Bhop - https://discord.gg/PfYMfbzMj7");
    bhop();
    return 0;
}
