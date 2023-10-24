#pragma once
#include "MovingObject.h"

class Laser : public MovingObject
{
public:
    Laser(float x, float y, float velocity);
    void Initialize();
    void Update(float dt);
    void Render();

private:
    size_t m_laserTexture;
};