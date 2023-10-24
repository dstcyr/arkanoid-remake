#pragma once
#include "Config.h"
#include "StateMachine.h"

class Intro : public IState
{
public:
    void OnEnter() override;
    void OnUpdate(float dt) override;
    void OnRender() override;
    void OnExit() override;

private:
    size_t m_whiteFont = 0;
    size_t m_orangeFont = 0;
    size_t m_introTheme = 0;
    float m_elapsedIntro = 0.0f;
    float m_delayIntro = 2.0f;
};