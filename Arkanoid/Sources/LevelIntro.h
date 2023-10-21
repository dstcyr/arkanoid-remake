#pragma once
#include "Config.h"
#include "StateMachine.h"

class LevelIntro : public IState
{
public:
    LevelIntro();
    void OnEnter() override;
    void OnUpdate(float dt) override;
    void OnRender() override;
    void OnExit() override;

private:
    size_t m_whiteFont;
    size_t m_orangeFont;
    float m_elapsedIntro;
    float m_delayIntro;
};