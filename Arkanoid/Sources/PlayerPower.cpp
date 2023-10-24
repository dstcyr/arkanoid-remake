#include "PlayerPower.h"
#include "Log.h"
#include "SaveGame.h"

PlayerPower::PlayerPower() 
{
}

PlayerPower::PlayerPower(float x, float y) : Power(x, y)
{
}

void PlayerPower::Init()
{
    Power::Init();
    m_animation.AddClip("fall", 30, 6, 0.1f);
    m_animation.Play("fall", true);
}

void PlayerPower::Activate()
{
    SaveGame::life++;
}

std::string PlayerPower::ToString()
{
    return "Player Capsule (5)";
}
