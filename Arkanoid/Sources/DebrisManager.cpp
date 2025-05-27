#include "DebrisManager.h"
#include "Game.h"
#include "utils/Stopwatch.h"
#include "GameLevel.h"
#include "maths/MathUtils.h"

void DebrisManager::Initialize()
{
    auto& audio = Game::Get().Audio();

    m_DebrisExplosionSfx = audio.LoadAudio("Audio/Explosion.wav");
    audio.SetVolume(m_DebrisExplosionSfx, 50);

    m_spawnElapsed = 0.0f;
    m_spawnCount = MathUtils::RandRange(1, 3);
    m_spawnInGame = MathUtils::RandRange(1, m_spawnCount);
    // BX_LOG(ELogLevel::Warning, "Will spawn %d debris", m_spawnCount);
    // BX_LOG(ELogLevel::Warning, "Initial spawn is %d", m_spawnInGame);

    Door topDoorA;
    topDoorA.anim = Animation();
    topDoorA.x = 225.0f;
    topDoorA.y = 10.0f;
    topDoorA.anim.Load("Images/topdoor.png");
    topDoorA.anim.Init(4, 48, 16);
    topDoorA.anim.AddClip("open", 0, 4, 0.1f);
    topDoorA.anim.AddClip("open_idle", 3, 1, 0.0f);
    topDoorA.anim.AddClip("close", 4, 4, 0.1f);
    topDoorA.anim.AddClip("close_idle", 7, 1, 0.0f);
    m_doors.push_back(topDoorA);

    Door topDoorB;
    topDoorB.anim = Animation();
    topDoorB.x = 575.0f;
    topDoorB.y = 10.0f;
    topDoorB.anim.Load("Images/topdoor.png");
    topDoorB.anim.Init(4, 48, 16);
    topDoorB.anim.AddClip("open", 0, 4, 0.1f);
    topDoorB.anim.AddClip("open_idle", 3, 1, 0.0f);
    topDoorB.anim.AddClip("close", 4, 4, 0.1f);
    topDoorB.anim.AddClip("close_idle", 7, 1, 0.0f);
    m_doors.push_back(topDoorB);
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
            // int debrisType = World::Get().GetDebrisTypeForCurrentLevel();
            // SpawnDebris(debrisType, 225.0f, 10.0f);
            m_taskMgr.Add(this, &DebrisManager::TaskSpawnDebris);
        }
        else if (m_spawnInGame < m_spawnCount)
        {
            // Got room to spawn more, try it:
            int randomNumber = rand() % 101;
            if (randomNumber >= 0 && randomNumber <= 45)
            {
                m_spawnInGame++;
                m_taskMgr.Add(this, &DebrisManager::TaskSpawnDebris);
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

    m_taskMgr.UpdateSequence(dt);
}

void DebrisManager::Render()
{
    for (Debris* debris : m_activeDebris)
    {
        debris->Render();
    }

    for (Explosion& explosion : m_exposions)
    {
        explosion.animation.Draw(explosion.transform);
    }

    m_doors[0].anim.Draw({201, 0, 98.0f, 34.0f});
    m_doors[1].anim.Draw({553, 0, 98.0f, 34.0f});
    // Engine::DrawCircle(m_doors[0].x, m_doors[0].y, 10.0f, NColor::Yellow);
    // Engine::DrawCircle(m_doors[1].x, m_doors[1].y, 10.0f, NColor::Red);
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
            exp.animation.Load("Images/explosion.png");
            exp.animation.Init(5, 32, 32);
            exp.animation.AddClip("play", 0, 5, 0.1f);
            exp.animation.Play("play", false);
            (*it)->GetTransform(&exp.transform);
            exp.transform.width = 64.0f;
            exp.transform.height = 64.0f;
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

void DebrisManager::CheckCollisions(LaserManager& lasers)
{
    auto it = m_activeDebris.begin();
    while (it != m_activeDebris.end())
    {
        if (lasers.CheckCollisionWith(**it))
        {
            Explosion exp;
            exp.animation = Animation();
            exp.animation.Load("Images/explosion.png");
            exp.animation.Init(5, 32, 32);
            exp.animation.AddClip("play", 0, 5, 0.1f);
            exp.animation.Play("play", false);
            (*it)->GetTransform(&exp.transform);
            exp.transform.width = 64.0f;
            exp.transform.height = 64.0f;
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
    auto& audio = Game::Get().Audio();

    static Stopwatch hitSFXStopwatch;
    static bool firstTime = true;

    hitSFXStopwatch.Stop();
    double elapsed = hitSFXStopwatch.ElapsedMilliseconds();
    if (elapsed > SFX_DELAY || firstTime)
    {
        audio.PlaySFX(m_DebrisExplosionSfx);
        hitSFXStopwatch.Start();
        firstTime = false;
    }
}

bool DebrisManager::TaskSpawnDebris(float dt, DebrisState* state)
{
    BX_CHECKS(state, "Invalid state");

    switch (state->phase)
    {
    case 0:
    {
        // Open one of the top doors:
        m_doors[state->door].anim.Play("open", false);
        state->phase++;
        break;
    }

    case 1:
    {
        // Wait for the animation complete:
        if (!m_doors[state->door].anim.Update(dt))
        {
            m_doors[state->door].anim.Play("open_idle", false);
            state->phase++;
        }

        break;
    }

    case 2:
    {
        // Spawn the debris
        Debris* newDebris = new Debris(GameLevel::Get().GetDebrisTypeForCurrentLevel());
        newDebris->Initialize();
        newDebris->SetPosition(m_doors[state->door].x, m_doors[state->door].y);
        m_activeDebris.push_back(newDebris);

        state->elapsed = 0.0f;
        state->phase++;
        break;
    }

    case 3:
    {
        // Give the debris some time to move out of the way
        state->elapsed += dt;
        if (state->elapsed >= 1.0f)
        {
            m_doors[state->door].anim.Play("close", false);
            state->elapsed = 0.0f;
            state->phase++;
        }
        break;
    }
    case 4:
    {
        // Wait until the door is closed 
        // (that prevents other task from starting before the door are closed)
        if (!m_doors[state->door].anim.Update(dt))
        {
            m_doors[state->door].anim.Play("close_idle", false);
            return false;
        }
        break;
    }
    }

    return true;
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
