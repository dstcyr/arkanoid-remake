#include "Laser.h"
#include "Config.h"
#include "Game.h"

Laser::Laser(float x, float y, float velocity)
{
    m_transform.x = x;
    m_transform.y = y;
    m_transform.width = LASER_WIDTH;
    m_transform.height = LASER_HEIGHT;
    m_velocity.x = 0.0f;
    m_velocity.y = velocity;
    m_laserTexture = 0;
}

void Laser::Initialize()
{
    auto& graphics = Game::Get().Graphics();
    m_laserTexture = graphics.LoadTexture("Images/laser.png");
}

void Laser::Update(float dt)
{
    m_transform.y -= m_velocity.y * dt;
}

void Laser::Render()
{
    auto& graphics = Game::Get().Graphics();
    graphics.DrawImage(m_laserTexture, m_transform, 255, 255, 255, 255);

#if SHOW_DEBUG_LASER
    Engine::DrawRect(m_transform.x, m_transform.y, m_transform.w, m_transform.h, NColor::Red);
#endif
}
