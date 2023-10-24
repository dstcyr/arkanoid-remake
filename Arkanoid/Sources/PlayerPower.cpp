#include "PlayerPower.h"
#include "Log.h"
#include "SaveGame.h"
#include "Engine.h"

PlayerPower::PlayerPower() 
{
}

PlayerPower::PlayerPower(float x, float y) : Power(x, y)
{
    m_LifeSfx = 0;
}

void PlayerPower::Init()
{
    Power::Init();
    m_LifeSfx = Engine::LoadSound("Assets/Audio/life.wav");
    Engine::SetVolume(m_LifeSfx, 30);

    m_animation.AddClip("fall", 30, 6, 0.1f);
    m_animation.Play("fall", true);
}

void PlayerPower::Activate()
{
    SaveGame::life++;
    Engine::PlaySFX(m_LifeSfx);
}

std::string PlayerPower::ToString()
{
    return "Player Capsule (5)";
}
