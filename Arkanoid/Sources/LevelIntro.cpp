#include "LevelIntro.h"
#include "Engine.h"
#include "SaveGame.h"

LevelIntro::LevelIntro()
{
    m_whiteFont = 0;
    m_orangeFont = 0;
    m_elapsedIntro = 0.0f;
    m_delayIntro = 2.0f;
}

void LevelIntro::OnEnter()
{
    m_elapsedIntro = 0.0f;
    m_whiteFont = Engine::LoadFont("Assets/Fonts/8bitwonder.ttf", "whitefont", 32, NColor::White);
    m_orangeFont = Engine::LoadFont("Assets/Fonts/8bitwonder.ttf", "redfont", 32, NColor(224, 80, 0, 255));
}

void LevelIntro::OnUpdate(float dt)
{
    m_elapsedIntro += dt;
    if(m_elapsedIntro >= m_delayIntro)
    {
        m_elapsedIntro = 0.0f;
        Engine::SetState("game");
    }
}

void LevelIntro::OnRender()
{
    Engine::DrawString("HIGH SCORE", m_orangeFont, 350.0f, 24.0f);
    Engine::DrawString("ROUND " + std::to_string(SaveGame::round), m_whiteFont, 350.0f, 580.0f);
}

void LevelIntro::OnExit()
{
}
