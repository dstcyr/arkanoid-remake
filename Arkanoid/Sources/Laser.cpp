#include "Laser.h"
#include "Config.h"
#include "Engine.h"

Laser::Laser(float x, float y, float velocity)
{
    m_transform.x = x;
    m_transform.y = y;
    m_transform.w = LASER_WIDTH;
    m_transform.h = LASER_HEIGHT;
    m_velocity.x = 0.0f;
    m_velocity.y = velocity;
    m_laserTexture = 0;
}

void Laser::Initialize()
{
    m_laserTexture = Engine::LoadTexture("Assets/Images/laser.png");
}

void Laser::Update(float dt)
{
    m_transform.y -= m_velocity.y * dt;
}

void Laser::Render()
{
    Engine::DrawTexture(m_laserTexture, m_transform);

#if SHOW_DEBUG_LASER
    Engine::DrawRect(m_transform.x, m_transform.y, m_transform.w, m_transform.h, NColor::Red);
#endif
}
