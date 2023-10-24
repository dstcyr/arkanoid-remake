#include "Intro.h"
#include "Engine.h"
#include "SaveGame.h"
#include "Log.h"

void Intro::OnEnter()
{
    LOG(LL_DEBUG, "Intro::OnEnter");
    m_elapsedIntro = 0.0f;
    m_whiteFont = Engine::LoadFont("Assets/Fonts/8bitwonder.ttf", "whitefont", 32, NColor::White);
    m_orangeFont = Engine::LoadFont("Assets/Fonts/8bitwonder.ttf", "redfont", 32, NColor(224, 80, 0, 255));
    m_introTheme = Engine::LoadMusic("Assets/Audio/round.wav");

    Engine::StopMusic();
    Engine::PlayMusic(m_introTheme, 0);
}

void Intro::OnUpdate(float dt)
{
    m_elapsedIntro += dt;
    if (m_elapsedIntro >= m_delayIntro)
    {
        m_elapsedIntro = 0.0f;
        Engine::SetState("game");
    }
}

void Intro::OnRender()
{
    Engine::DrawString("HIGH SCORE", m_orangeFont, 350.0f, 24.0f);
    Engine::DrawString("ROUND " + std::to_string(SaveGame::round), m_whiteFont, 350.0f, 580.0f);
}

void Intro::OnExit()
{
}
