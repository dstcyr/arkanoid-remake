#pragma once
#include "Config.h"
#include "screens/ScreenState.h"

class Intro : public ScreenState
{
public:
    void Enter() override;
    void Update(float dt) override;
    void Draw() override;
    void Exit() override;

private:
    size_t m_whiteFont = 0;
    size_t m_orangeFont = 0;
    float m_elapsedIntro = 0.0f;
    float m_delayIntro = 2.0f;
};