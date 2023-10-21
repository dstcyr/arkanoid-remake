#pragma once
#include "Config.h"
#include "StateMachine.h"

class Title : public IState
{
public:
    Title();
    void OnEnter() override;
    void OnUpdate(float dt) override;
    void OnRender() override;
    void OnExit() override;

private:
    size_t m_whiteFont;
    size_t m_orangeFont;
    size_t m_title;
    size_t m_titleMusic;
};