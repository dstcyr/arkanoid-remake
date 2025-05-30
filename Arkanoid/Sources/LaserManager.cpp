#include "LaserManager.h"
#include "Config.h"
#include "GameLevel.h"
#include "Game.h"

void LaserManager::Initialize()
{
    auto& audio = Game::Get().Audio();
    m_LaserSfx = audio.LoadAudio("Audio/laser.wav");
    audio.SetVolume(m_LaserSfx, 50);
}

void LaserManager::SpawnLaser(float x1, float y1, float x2, float y2)
{
    auto& audio = Game::Get().Audio();

    Laser* laserA = new Laser(x1, y1, LASER_SPEED);
    laserA->Initialize();

    Laser* laserB = new Laser(x2, y2, LASER_SPEED);
    laserB->Initialize();

    m_activateLasers.push_back(laserA);
    m_activateLasers.push_back(laserB);
    audio.PlaySFX(m_LaserSfx);
}

void LaserManager::Update(float dt)
{
    std::vector<Laser*>::iterator it = m_activateLasers.begin();
    while (it != m_activateLasers.end())
    {
        Laser* current = *it;
        current->Update(dt);

        int hitIndex;
        bool collides = GameLevel::Get().CheckCollision(current->GetX(), current->GetY(), current->GetW(), current->GetH(), &hitIndex);
        if (collides)
        {
#if USE_SOUNDFX
                Engine::PlaySFX(m_brickHitSound);
#endif
                GameLevel::Get().HitTile(hitIndex);
        }

        if (current->GetY() < TOP_WALL_Y || collides)
        {
            it = m_activateLasers.erase(it);
        }
        else
        {
            it++;
        }
    }
}

void LaserManager::Render()
{
    for (Laser* laser : m_activateLasers)
    {
        laser->Render();
    }
}

void LaserManager::Clear()
{
    for (Laser* laser : m_activateLasers)
    {
        SAFE_DELETE(laser);
    }

    m_activateLasers.clear();
}

bool LaserManager::CheckCollisionWith(MovingObject& other)
{
    std::vector<Laser*>::iterator it = m_activateLasers.begin();
    while (it != m_activateLasers.end())
    {
        Laser* current = *it;
        if (current->CheckCollisionWith(other))
        {
            it = m_activateLasers.erase(it);
            return true;
        }
        else
        {
            it++;
        }
    }

    return false;
}
