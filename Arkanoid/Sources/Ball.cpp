#include "Ball.h"
#include "Engine.h"
#include "Grid.h"
#include "Log.h"
#include "MathUtils.h"
#include "Collision.h"

Ball::Ball()
{
    m_transform = Rect<float>(400.0f, 650.0f, 16.0f, 16.0f);
    m_velocity = Vec2D(0.0f, 0.0f);
    m_ballTexture = 0;
    m_brickHitSound = 0;
}

void Ball::Initialize()
{
    m_ballTexture = Engine::LoadTexture("Assets/Images/Ball.png");
    m_brickHitSound = Engine::LoadSound("Assets/Audio/BrickHit.wav");
    SetAngle(SHARP_ANGLE);
}

void Ball::Update(float dt, const Rect<float>& bounds, Grid& grid)
{
    // prediction for the new position:
    float pX = m_transform.x + m_velocity.x * dt;
    float pY = m_transform.y + m_velocity.y * dt;

    // check bounds collision and update the predicted position:
    CheckBallCollisionWithBounds(bounds, &pX, &pY);

    // if the predicted position does not hit a block, move the ball
    // to that position:
    if (!CheckBallCollisionWithGrid(grid, pX, pY))
    {
        m_transform.x = pX;
        m_transform.y = pY;
    }
}

void Ball::Render()
{
    Engine::DrawTexture(m_ballTexture, m_transform);

#if SHOW_BALL_DEBUG
    Engine::FillRect(m_transform.x, m_transform.y, m_transform.w, m_transform.h, NColor::Yellow);
#endif

#if SHOW_BALL_DIRECTION_LINE
    Engine::DrawLine(m_transform.x + m_transform.w / 2.0f, 0.0f, m_transform.x + m_transform.w / 2.0f, 1000.0f, NColor::LightLavender);
    Engine::DrawLine(0.0f, m_transform.y + m_transform.h / 2.0f, 1500.0f, m_transform.y + m_transform.h / 2.0f, NColor::LightLavender);
#endif
}

void Ball::SetAngle(float angle)
{
    m_velocity.SetRotation(angle);
}

void Ball::ChangeDirection(int h, int v)
{
    h = Engine::Clamp(h, -1, 1);
    v = Engine::Clamp(v, -1, 1);

    if (h != 0)
    {
        m_velocity.x = h * std::abs(m_velocity.x);
    }
    else
    {
        // m_velocity.x = -m_velocity.x;
    }

    if (v != 0)
    {
        m_velocity.y = v * std::abs(m_velocity.y);
    }
    else
    {
        // m_velocity.y = -m_velocity.y;
    }
}

void Ball::SetPosition(float x, float y)
{
    m_transform.x = x;
    m_transform.y = y;
}

void Ball::GetTransform(Rect<float>* transform)
{
    *transform = m_transform;
}

void Ball::GetVelocity(Vec2D* velocity)
{
    *velocity = m_velocity;
}

void Ball::SetSpeed(float speed)
{
    Vec2D dir = m_velocity;
    float magnitude = dir.Length();

    if (magnitude == 0.0f)
    {
        m_velocity.x = speed;
        m_velocity.y = speed;
        SetAngle(SHARP_ANGLE);
        return;
    }

    dir.Normalize();

    m_velocity.x = dir.x * speed;
    m_velocity.y = dir.y * speed;
}

void Ball::CheckBallCollisionWithBounds(const Rect<float>& bounds, float* px, float* py)
{
    float halfWidth = m_transform.w / 2.0f;
    float halfHeight = m_transform.h / 2.0f;

    if (*px > bounds.w - halfWidth)
    {
        m_velocity.x = -m_velocity.x;
        *px = bounds.w - halfWidth;
    }
    else if (*px < bounds.x)
    {
        m_velocity.x = -m_velocity.x;
        *px = bounds.x;
    }
    
    if (*py > bounds.h - halfHeight)
    {
        m_velocity.y = -m_velocity.y;
        *py = bounds.h - halfHeight;
        OnBottomReached.Invoke<BallEvent>(this, *px, *py);
    }
    else if (*py < bounds.y)
    {
        m_velocity.y = -m_velocity.y;
        *py = bounds.y;
    }
}

bool Ball::CheckBallCollisionWithGrid(Grid& grid, float px, float py)
{
    int hitIndex = 0;

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

        if (destroyed)
        {
            OnBlockDestroyed.Invoke<BallEvent>(this, static_cast<float>(worldX), static_cast<float>(worldY));
        }

        bool bottom = (worldY < m_transform.y + m_transform.h);
        bool top = (worldY + grid.GetCellHeight() > m_transform.y);
        bool right = (worldX <= m_transform.x + m_transform.w);
        bool left = (worldX + grid.GetCellWidth() > m_transform.x);

        // TODO (REVIEW) : optimize boolean expression
        // TODO (REVIEW) : see CheckCollisionWithRect... 
        // 
        // The ball hits the top of a block
        if (top && !bottom)
        {
            m_velocity.y = -m_velocity.y;
        }

        // The ball hits the bottom of a block
        if (bottom && !top)
        {
            m_velocity.y = -m_velocity.y;
        }

        // The ball hits a block from the left
        if (left && !right)
        {
            m_velocity.x = -m_velocity.x;
        }

        // The ball hits a block from the right
        if (right && !left)
        {
            m_velocity.x = -m_velocity.x;
        }

        return true;
    }

    return false;
}

bool Ball::CheckCollisionWithRect(const Rect<float>& rect)
{
    int hitIndex = 0;
    if(Engine::CheckRects(m_transform, rect))
    {
        float centerX = m_transform.x + (m_transform.w / 2.0f);
        float centerY = m_transform.y + (m_transform.h / 2.0f);

        bool top = centerY < rect.y;
        bool bottom = centerY > rect.y + rect.h;
        bool left = centerX < rect.x;
        bool right = centerX > rect.x + rect.w;

        // TODO (REVIEW) : optimize boolean expression
        // 
        // The ball hits the top of a block
        if (top && !bottom)
        {
            m_velocity.y = -m_velocity.y;
        }

        // The ball hits the bottom of a block
        if (bottom && !top)
        {
            m_velocity.y = -m_velocity.y;
        }

        // The ball hits a block from the left
        if (left && !right)
        {
            m_velocity.x = -m_velocity.x;
        }

        // The ball hits a block from the right
        if (right && !left)
        {
            m_velocity.x = -m_velocity.x;
        }

        return true;
    }

    return false;
}
