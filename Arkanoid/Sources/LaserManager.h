#pragma once
#include <vector>
#include "Laser.h"

class LaserManager
{
public:
    void Initialize();
    void SpawnLaser(float x1, float y1, float x2, float y2);
    void Update(float dt);
    void Render();
    void Clear();

private:
    std::vector<Laser*> m_activateLasers;
    size_t m_LaserSfx;
};