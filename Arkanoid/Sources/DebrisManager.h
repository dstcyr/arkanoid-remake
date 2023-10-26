#pragma once
#include <vector>
#include "Debris.h"
#include "Animation.h"

struct Explosion
{
    Animation animation;
    Rect<float> transform;
};

class DebrisManager
{
public:
    void Initialize();
    void SpawnDebris(int type, float x, float y);
    void Update(float dt);
    void Render();
    void CheckCollisions(BallManager& balls);
    void Clear();

private:
    std::vector<Debris*> m_activeDebris;
    std::vector<Explosion> m_exposions;
    float m_spawnElapsed;
    int m_spawnCount;
    int m_spawnInGame;
    size_t m_DebrisExplosionSfx;

    void PlayExplosionSFX();
};