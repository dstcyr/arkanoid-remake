#include "LaserManager.h"
#include "Config.h"
#include "World.h"
#include "Engine.h"

void LaserManager::Initialize()
{
    m_LaserSfx = Engine::LoadSound("Assets/Audio/laser.wav");
    Engine::SetVolume(m_LaserSfx, 50);
}

void LaserManager::SpawnLaser(float x1, float y1, float x2, float y2)
{
    Laser* laserA = new Laser(x1, y1, LASER_SPEED);
    laserA->Initialize();

    Laser* laserB = new Laser(x2, y2, LASER_SPEED);
    laserB->Initialize();

    m_activateLasers.push_back(laserA);
    m_activateLasers.push_back(laserB);
    Engine::PlaySFX(m_LaserSfx);
}

void LaserManager::Update(float dt)
{
    std::vector<Laser*>::iterator it = m_activateLasers.begin();
    while (it != m_activateLasers.end())
    {
        Laser* current = *it;
        current->Update(dt);

        int hitIndex;
        bool collides = World::Get().CheckCollision(current->GetX(), current->GetY(), current->GetW(), current->GetH(), &hitIndex);
        if (collides)
        {
#if USE_SOUNDFX
                Engine::PlaySFX(m_brickHitSound);
#endif
                World::Get().HitTile(hitIndex);
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
