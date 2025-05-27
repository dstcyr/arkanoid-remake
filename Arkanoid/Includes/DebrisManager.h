#pragma once
#include <vector>
#include "Debris.h"
#include "graphics/Animation.h"
#include "utils/TaskManager.h"
#include "maths/MathUtils.h"
#include "LaserManager.h"

struct Explosion
{
    Animation animation;
    Rect<float> transform;
};

struct Door
{
    Animation anim;
    float x;
    float y;
};

class DebrisState : public CTaskState
{
public:
    DebrisState()
    {
        elapsed = 0.0f;
        phase = 0;
        firstPass = true;
        door = MathUtils::RandRange(0, 1);
    }

    float elapsed;
    int phase;
    bool firstPass;
    int door;
};

class DebrisManager
{
public:
    void Initialize();
    void Update(float dt);
    void Render();
    void CheckCollisions(BallManager& balls);
    void CheckCollisions(LaserManager& lasers);
    void Clear();

private:
    std::vector<Debris*> m_activeDebris;
    std::vector<Explosion> m_exposions;
    std::vector<Door> m_doors;
    float m_spawnElapsed;
    int m_spawnCount;
    int m_spawnInGame;
    size_t m_DebrisExplosionSfx;
    CTaskManager m_taskMgr;

    void PlayExplosionSFX();

    bool TaskSpawnDebris(float dt, DebrisState* state);
};
