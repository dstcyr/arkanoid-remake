#include "DebrisManager.h"
#include "Engine.h"
#include "Stopwatch.h"
#include "World.h"
#include "MathUtils.h"
#include "Log.h"

void DebrisManager::Initialize()
{
    m_DebrisExplosionSfx = Engine::LoadSound("Assets/Audio/explosion.wav");
    Engine::SetVolume(m_DebrisExplosionSfx, 50);

    m_spawnElapsed = 0.0f;
    m_spawnCount = Engine::RandRange(1, 3);
    m_spawnInGame = Engine::RandRange(1, m_spawnCount);
    LOG(LL_WARNING, "Will spawn %d debris", m_spawnCount);
    LOG(LL_WARNING, "Initial spawn is %d", m_spawnInGame);
}

void DebrisManager::SpawnDebris(int type, float x, float y)
{
    Debris* newDebris = new Debris(type);
    newDebris->Initialize();
    newDebris->SetPosition(x, y);
    m_activeDebris.push_back(newDebris);
}

void DebrisManager::Update(float dt)
{
    m_spawnElapsed += dt;
    if (m_spawnElapsed >= 4.0f)
    {
        m_spawnElapsed = 0.0f;
        if (m_activeDebris.size() < m_spawnInGame)
        {
            // Missing a debris, respawn it right away
            int debrisType = World::Get().GetDebrisTypeForCurrentLevel();
            SpawnDebris(debrisType, 225.0f, 10.0f);
        }
        else if (m_spawnInGame < m_spawnCount)
        {
            // Got room to spawn more, try it:
            int randomNumber = rand() % 101;
            if (randomNumber >= 0 && randomNumber <= 45)
            {
                m_spawnInGame++;
                int debrisType = World::Get().GetDebrisTypeForCurrentLevel();
                SpawnDebris(debrisType, 225.0f, 10.0f);
            }
        }
    }

    for (Debris* debris : m_activeDebris)
    {
        debris->Update(dt);
    }

    auto it = m_exposions.begin();
    while (it != m_exposions.end())
    {
        bool done = !(*it).animation.Update(dt);
        if (done)
        {
            it = m_exposions.erase(it);
        }
        else
        {
            it++;
        }
    }
}

void DebrisManager::Render()
{
    for (Debris* debris : m_activeDebris)
    {
        debris->Render();
    }

    for (Explosion& explosion : m_exposions)
    {
        explosion.animation.Render(explosion.transform);
    }
}

void DebrisManager::CheckCollisions(BallManager& balls)
{
    auto it = m_activeDebris.begin();
    while (it != m_activeDebris.end())
    {
        if (balls.CheckCollisionWith(**it))
        {
            Explosion exp;
            exp.animation = Animation();
            exp.animation.Init("Assets/Images/explosion.png", 5, 32, 32);
            exp.animation.AddClip("play", 0, 5, 0.1f);
            exp.animation.Play("play", false);
            (*it)->GetTransform(&exp.transform);
            exp.transform.w = 64.0f;
            exp.transform.h = 64.0f;
            m_exposions.push_back(exp);
            PlayExplosionSFX();

            it = m_activeDebris.erase(it);
        }
        else
        {
            it++;
        }
    }
}

void DebrisManager::PlayExplosionSFX()
{
    static CStopwatch hitSFXStopwatch;
    static bool firstTime = true;

    hitSFXStopwatch.Stop();
    double elapsed = hitSFXStopwatch.ElapsedMilliseconds();
    if (elapsed > SFX_DELAY || firstTime)
    {
        Engine::PlaySFX(m_DebrisExplosionSfx);
        hitSFXStopwatch.Start();
        firstTime = false;
    }
}

void DebrisManager::Clear()
{
    for (Debris* debris : m_activeDebris)
    {
        SAFE_DELETE(debris);
    }

    m_exposions.clear();
    m_activeDebris.clear();
}
