#include "Screens/Menu.h"
#include "SaveGame.h"
#include "Game.h"

void Menu::Enter()
{
    BX_LOG(ELogLevel::Log, "Menu::OnEnter");

    auto& graphics = Game::Get().Graphics();
    auto& audio = Game::Get().Audio();

    m_whiteFont = graphics.LoadFont("Fonts/8bitwonder.ttf", 32);
    m_orangeFont = graphics.LoadFont("Fonts/8bitwonder.ttf", 32);
    m_title = graphics.LoadTexture("Images/title.png");
    m_titleMusic = audio.LoadAudio("Audio/theme.wav");

#if MENU_MOCKUP
    m_menuMockup = Engine::LoadTexture("Design/menu.png");
#endif

    audio.StopMusic();
    audio.PlaySFX(m_titleMusic);
}

void Menu::Update(float dt)
{
    auto& inputs = Game::Get().Inputs();
    auto& world = Game::Get().World();

    if (inputs.IsKeyDown(EKey::EKEY_RETURN))
    {
        SaveGame::Load();
        world.ChangeState("intro");
    }
}

void Menu::Draw()
{
    auto& graphics = Game::Get().Graphics();

#if MENU_MOCKUP
    Engine::DrawTexture(m_menuMockup, 0.0f, 0.0f, NColor(255, 255, 255, 50));
#endif

    graphics.DrawImage(m_title, { 260, 240, 494, 101 }, 255, 255, 255, 255);
    graphics.DrawString("HIGH SCORE", m_orangeFont, 350.0f, 30.0f, 255, 255, 255, 255);
    graphics.DrawString(std::to_string(SaveGame::highScore), m_whiteFont, 450.0f, 70.0f, 255, 255, 255, 255);
    graphics.DrawString("1UP", m_orangeFont, 140.0f, 30.0f, 255, 255, 255, 255);
    graphics.DrawString(std::to_string(SaveGame::score), m_whiteFont, 180.0f, 70.0f, 255, 255, 255, 255);
    graphics.DrawString("PUSH START", m_whiteFont, 350.0f, 550.0f, 255, 255, 255, 255);
}

void Menu::Exit()
{
    auto& audio = Game::Get().Audio();
    audio.StopAllSounds();
}
