#include "PlayerPower.h"
#include "SaveGame.h"
#include "Game.h"

PlayerPower::PlayerPower() 
{
}

PlayerPower::PlayerPower(float x, float y) : Power(x, y)
{
    m_LifeSfx = 0;
}

void PlayerPower::Init()
{
    auto& audio = Game::Get().Audio();

    Power::Init();
    m_LifeSfx = audio.LoadAudio("Audio/life.wav");
    audio.SetVolume(m_LifeSfx, 30);

    m_animation.AddClip("fall", 30, 6, 0.1f);
    m_animation.Play("fall", true);
}

void PlayerPower::Activate()
{
    auto& audio = Game::Get().Audio();

    SaveGame::life++;
    audio.PlaySFX(m_LifeSfx);
}

std::string PlayerPower::ToString()
{
    return "Player Capsule (5)";
}
