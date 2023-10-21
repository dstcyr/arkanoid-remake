#include "Laser.h"
#include "Engine.h"
#include "Grid.h"

Laser::Laser() : Laser(0.0f, 0.0f, 0.0f)
{
    m_laserTexture = 0;
}

Laser::Laser(float x, float y, float v)
{
    m_transform.x = x;
    m_transform.y = y;
    m_transform.w = LASER_WIDTH;
    m_transform.h = LASER_HEIGHT;
    m_velocity = v;
    m_laserTexture = 0;
}

void Laser::Initialize()
{
    m_laserTexture = Engine::LoadTexture("Assets/Images/laser.png");
}

void Laser::Update(float dt)
{
    m_transform.y -= m_velocity * dt;
}

void Laser::Render()
{
    Engine::DrawTexture(m_laserTexture, m_transform);

#if SHOW_DEBUG_LASER
    Engine::DrawRect(m_transform.x, m_transform.y, m_transform.w, m_transform.h, NColor::Red);
#endif
}

void Laser::GetTransform(Rect<float>* transform)
{
    *transform = m_transform;
}

// TODO (REVIEW) : code repetition, create a movable class ?
bool Laser::CheckBallCollisionWithGrid(Grid& grid)
{
    int hitIndex = 0;
    float px = m_transform.x;
    float py = m_transform.y;

    // if there is a collision with blocks in the grid, this function
    // returns the closests id using the distance between both rectangle's
    // centers. The ball can hit only one target at a time:
    if (grid.CheckCollision(px, py, m_transform.w, m_transform.h, &hitIndex))
    {
#if USE_SOUNDFX
        Engine::PlaySFX(m_brickHitSound);
#endif

        bool destroyed = grid.Hit(hitIndex);

        int worldX, worldY;
        grid.GetWorldPositionFromIndex(hitIndex, &worldX, &worldY);
        return true;
    }

    return false;
}