#include "Title.h"
#include "Engine.h"

Title::Title()
{
    m_whiteFont = 0;
    m_orangeFont = 0;
    m_title = 0;
}

void Title::OnEnter()
{
    m_whiteFont = Engine::LoadFont("Assets/Fonts/8bitwonder.ttf", "whitefont", 32, NColor::White);
    m_orangeFont = Engine::LoadFont("Assets/Fonts/8bitwonder.ttf", "redfont", 32, NColor(224, 80, 0, 255));
    m_title = Engine::LoadTexture("Assets/Images/Title.png");
}

void Title::OnUpdate(float dt)
{
    if (Engine::GetKeyDown(KEY_START))
    {
        Engine::SetState("intro");
    }
}

void Title::OnRender()
{
    Engine::DrawTexture(m_title, 260, 240, 494, 101);
    Engine::DrawString("HIGH SCORE", m_orangeFont, 350.0f, 24.0f);
    Engine::DrawString("PUSH START", m_whiteFont, 350.0f, 580.0f);
}

void Title::OnExit()
{
}
