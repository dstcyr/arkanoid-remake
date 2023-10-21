#pragma once
#include "Rect.h"
#include "Config.h"

class Grid;

class Laser
{
public:
    Laser();
    Laser(float x, float y, float v);
    void Initialize();
    void Update(float dt);
    void Render();
    void GetTransform(Rect<float>* transform);
    bool CheckBallCollisionWithGrid(Grid& grid);

private:
    Rect<float> m_transform;
    float m_velocity;
    size_t m_laserTexture;
};