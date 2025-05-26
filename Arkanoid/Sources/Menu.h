#pragma once
#include "Config.h"
#include "screens/ScreenState.h"

class Menu : public ScreenState
{
public:
    void Enter() override;
    void Update(float dt) override;
    void Draw() override;
    void Exit() override;

private:
    size_t m_whiteFont = 0;
    size_t m_orangeFont = 0;
    size_t m_title = 0;
    size_t m_titleMusic = 0;

#if MENU_MOCKUP
    size_t m_menuMockup = 0;
#endif
};