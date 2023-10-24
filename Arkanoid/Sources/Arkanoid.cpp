#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <iostream>
#include <conio.h>
#include "StateMachine.h"
#include "Engine.h"
#include <string>
#include "File.h"
#include "Log.h"
#include "Config.h"
#include "Menu.h"
#include "Intro.h"
#include "Game.h"

#if CHECK_MEMORY_LEAKS
#pragma comment(lib, "C:\\vld\\lib\\Win64\\vld.lib")
#include "C:\\vld\\include\\vld.h"
#endif

// https://www.world-of-nintendo.com/manuals/nes/arkanoid.shtml
// https://youtu.be/ckyQe_EBJG4?si=9JIr7HXkcIzKXcxc
// https://strategywiki.org/wiki/Arkanoid/Gameplay
// https://www.spriters-resource.com/nes/arkanoid/
// https://www.informit.com/articles/article.aspx?p=2180418&seqNum=2
INT WINAPI WinMain(HINSTANCE _In_ hInst, HINSTANCE _In_opt_ hPrev, PSTR _In_ cmd, INT _In_ flag)
{
    std::string title = "Arkanoid Remake";

#if CREATE_DEBUG_CONSOLE
    AllocConsole();
    int _r = freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
    if (_r != 0)
    {
        OutputDebugString("error: can't allocate console");
    }

    title += " - DEBUG";
#endif

    StateMachine fsm;
    fsm.Add("menu", new Menu());
    fsm.Add("intro", new Intro());
    fsm.Add("game", new Game());

    Engine::Initialize(title.c_str(), SCREEN_WIDTH, SCREEN_HEIGHT);

#if START_IN_GAME
    fsm.SetState("game");
#elif START_IN_INTRO
    fsm.SetState("intro");
#else
    fsm.SetState("menu");
#endif

    Engine::EnterGameLoop(&fsm);

#if CREATE_DEBUG_CONSOLE
#if PAUSE_CONSOLE_WHEN_CLOSING
    LOG(LL_VERBOSE, "Press any key to continue...");
    int r = _getch();
#endif
    FreeConsole();
#endif

    return 0;
}
