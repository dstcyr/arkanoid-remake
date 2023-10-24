#pragma once
#include "Config.h"
#include "StateMachine.h"

class Menu : public IState
{
public:
    void OnEnter() override;
    void OnUpdate(float dt) override;
    void OnRender() override;
    void OnExit() override;

private:
    size_t m_whiteFont = 0;
    size_t m_orangeFont = 0;
    size_t m_title = 0;
    size_t m_titleMusic = 0;

#if MENU_MOCKUP
    size_t m_menuMockup = 0;
#endif
};