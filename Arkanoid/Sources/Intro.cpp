#include "Intro.h"
#include "SaveGame.h"
#include "Game.h"

void Intro::Enter()
{
    BX_LOG(ELogLevel::Log, "Intro::OnEnter");

    auto& graphics = Game::Get().Graphics();

    m_elapsedIntro = 0.0f;
    m_whiteFont = graphics.LoadFont("Fonts/8bitwonder.ttf", 32);
    m_orangeFont = graphics.LoadFont("Fonts/8bitwonder.ttf", 32);
}

void Intro::Update(float dt)
{
    auto& world = Game::Get().World();

    m_elapsedIntro += dt;
    if (m_elapsedIntro >= m_delayIntro)
    {
        m_elapsedIntro = 0.0f;
        world.ChangeState("game");
    }
}

void Intro::Draw()
{
    auto& graphics = Game::Get().Graphics();

    graphics.DrawString("HIGH SCORE", m_orangeFont, 350.0f, 30.0f, 255, 255, 255, 255);
    graphics.DrawString(std::to_string(SaveGame::highScore), m_whiteFont, 450.0f, 70.0f, 255, 255, 255, 255);
    graphics.DrawString("1UP", m_orangeFont, 140.0f, 30.0f, 255, 255, 255, 255);
    graphics.DrawString(std::to_string(SaveGame::score), m_whiteFont, 180.0f, 70.0f, 255, 255, 255, 255);
    graphics.DrawString("ROUND " + std::to_string(SaveGame::round), m_whiteFont, 400.0f, 480.0f, 255, 255, 255, 255);
}

void Intro::Exit()
{
}
