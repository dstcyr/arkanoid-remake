#include "Menu.h"
#include "Engine.h"
#include "SaveGame.h"

void Menu::OnEnter()
{
    m_whiteFont = Engine::LoadFont("Assets/Fonts/8bitwonder.ttf", "whitefont", 32, NColor::White);
    m_orangeFont = Engine::LoadFont("Assets/Fonts/8bitwonder.ttf", "redfont", 32, NColor(224, 80, 0, 255));
    m_title = Engine::LoadTexture("Assets/Images/Title.png");
    m_titleMusic = Engine::LoadMusic("Assets/Audio/Menu.wav");

#if MENU_MOCKUP
    m_menuMockup = Engine::LoadTexture("Assets/Design/menu.png");
#endif

    Engine::PlayMusic(m_titleMusic, false);
}

void Menu::OnUpdate(float dt)
{
    if (Engine::GetKeyDown(KEY_START))
    {
        SaveGame::Load();
        Engine::SetState("intro");
    }
}

void Menu::OnRender()
{
#if MENU_MOCKUP
    Engine::DrawTexture(m_menuMockup, 0.0f, 0.0f, NColor(255, 255, 255, 50));
#endif

    Engine::DrawTexture(m_title, 260, 240, 494, 101);
    Engine::DrawString("HIGH SCORE", m_orangeFont, 350.0f, 30.0f);
    Engine::DrawString(std::to_string(SaveGame::highScore), m_whiteFont, 450.0f, 70.0f);
    Engine::DrawString("1UP", m_orangeFont, 140.0f, 30.0f);
    Engine::DrawString(std::to_string(SaveGame::score), m_whiteFont, 180.0f, 70.0f);
    Engine::DrawString("PUSH START", m_whiteFont, 350.0f, 550.0f);
}

void Menu::OnExit()
{
    Engine::StopMusic();
}
