#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <iostream>
#include <conio.h>
#include "StateMachine.h"
#include "Engine.h"
#include <string>
#include "File.h"
#include "Log.h"

#if 0
#pragma comment(lib, "C:\\vld\\lib\\Win64\\vld.lib")
#include "C:\\vld\\include\\vld.h"
#endif

// https://www.world-of-nintendo.com/manuals/nes/arkanoid.shtml
INT WINAPI WinMain(HINSTANCE _In_ hInst, HINSTANCE _In_opt_ hPrev, PSTR _In_ cmd, INT _In_ flag)
{
    std::string title = "Arkanoid Remake";

#if defined(_DEBUG)
    AllocConsole();
    int _r = freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
    if (_r != 0)
    {
        OutputDebugString("error: can't allocate console");
    }

    title += " - DEBUG";
#endif

    StateMachine fsm;
    //fsm.Add("menu", new MenuState());
    //fsm.Add("option", new OptionState());
    //fsm.Add("game", new GameState());

    Engine::Initialize(title.c_str(), 1024, 962);
    //fsm.SetState("menu");

    Engine::EnterGameLoop(&fsm);

#if defined(_DEBUG)
    // LOG(LL_VERBOSE, "Press any key to continue...");
    // int r = _getch();
    FreeConsole();
#endif

    return 0;
}
