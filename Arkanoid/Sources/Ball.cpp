#include "Ball.h"
#include "Engine.h"
#include "Log.h"
#include "Ship.h"
#include "World.h"
#include "Collision.h"
#include "Stopwatch.h"
#include "Config.h"

Ball::Ball() : Ball(BALL_START_X, BALL_START_Y)
{
}

Ball::Ball(float x, float y)
{
    m_transform.x = x;
    m_transform.y = y;
    m_transform.w = 16.0f;
    m_transform.h = 16.0f;
}

void Ball::Initialize()
{
    m_ballTexture = Engine::LoadTexture("Assets/Images/Ball.png");
    m_shipCollisionSfx = Engine::LoadSound("Assets/Audio/shiphit.wav");
    Engine::SetVolume(m_shipCollisionSfx, 50);
    m_blockCollisionSfx = Engine::LoadSound("Assets/Audio/blockhit.wav");
    Engine::SetVolume(m_blockCollisionSfx, 50);
    m_hardBlockCollisionSfx = Engine::LoadSound("Assets/Audio/hardblockhit.wav");
    Engine::SetVolume(m_hardBlockCollisionSfx, 50);
}

void Ball::Update(float dt)
{
    // prediction for the new position:
    float pX = m_transform.x + m_velocity.x * dt;
    float pY = m_transform.y + m_velocity.y * dt;

    // check bounds collision and update the predicted position:
    if (!CheckCollisionWithBounds(&pX, &pY))
    {
        float collided = CheckCollisionWithShip(&pX, &pY);

        if (!collided)
        {
            collided = CheckCollisionWithGrid(&pX, &pY);
        }

        if (collided)
        {
            OnBounced.Invoke<BallEvent>(this, pX, pY);
        }
    }

    m_transform.x = pX;
    m_transform.y = pY;
}

void Ball::Render()
{
    Engine::DrawTexture(m_ballTexture, m_transform);

#if SHOW_BALL_DEBUG
    Engine::DrawLine(m_transform.x + m_transform.w / 2.0f, 0.0f, m_transform.x + m_transform.w / 2.0f, 1000.0f, NColor::LightLavender);
    Engine::DrawLine(0.0f, m_transform.y + m_transform.h / 2.0f, 1500.0f, m_transform.y + m_transform.h / 2.0f, NColor::LightLavender);
    Engine::DrawRect(m_transform.x, m_transform.y, m_transform.w, m_transform.h, NColor(0, 255, 0, 255));
#endif
}

void Ball::SetAngle(float angle)
{
    float length = m_velocity.Length();
    if (length == 0.0f)
    {
        m_velocity.Set(0.0f, 1.0f);
        length = 1.0f;
    }

    m_velocity.Normalize();
    m_velocity.SetRotation(angle);
    m_velocity *= length;
}

void Ball::SetSpeed(float speed)
{
    m_velocity.Normalize();
    m_velocity *= speed;
}

float Ball::GetAngle() const
{
    return m_velocity.GetAngle();
}

bool Ball::CheckCollisionWithBounds(float* px, float* py)
{
    bool result = false;
    if (*px > RIGHT_WALL_X - m_transform.w)
    {
        m_velocity.CCW();
        *px = RIGHT_WALL_X - m_transform.w;
        result = true;
    }
    else if (*px < LEFT_WALL_X)
    {
        m_velocity.CCW();
        *px = LEFT_WALL_X;
        result = true;
    }

    if (*py > BOTTOM_WALL_Y - m_transform.h)
    {
        m_velocity.CW();
        *py = BOTTOM_WALL_Y - m_transform.h;
        result = true;
        OnBottomReached.Invoke<BallEvent>(this, *px, *py);
    }
    else if (*py < TOP_WALL_Y)
    {
        m_velocity.CW();
        *py = TOP_WALL_Y;
        result = true;
    }

    return result;
}

bool Ball::CheckCollisionWithShip(float* px, float* py)
{
    // Where the ball hits the paddle dictates how it will bounce off the paddle.
    // If the ball hits the silver area in the middle, it will bounce off at a sharp angle.
    // If it hits the red bands near the sides, it will bounce off at a 45 degree angle.
    // And if it hits the very edges of the paddle, it will bounce off at a very shallow angle.
    // Use this information to not only keep the ball in play, but to better direct the path
    // that it takes while you attempt to knock out all of the bricks.

    // The ball initial launch is at 65°
    // If the ball hit neer the middle of the ship, it's launched at 65°
    // If the ball hit near the extremities of the ship, it's launched at 45°
    // If the ball hit near the sides of the ship, it's launched at 20°

    Ship* ship = World::Get().GetShip();
    CHECK(ship);

    float halfWidth = m_transform.w / 2.0f;
    float halfHeight = m_transform.h / 2.0f;
    float centerX = m_transform.x + halfWidth;
    float centerY = m_transform.y + halfHeight;

    if (CheckCollisionWith(*ship))
    {
        ECollisionResult response = ship->GetCollisionResponse(centerX, centerY);
        switch (response)
        {
        case LEFT_SIDE_HIT:
            m_velocity.SetRotation(-150.0f);
            break;
        case RIGHT_SIDE_HIT:
            m_velocity.SetRotation(-30.0f);
            break;
        case LEFT_ZONE_HIT:
            m_velocity.SetRotation(-135.0f);
            break;
        case RIGHT_ZONE_HIT:
            m_velocity.SetRotation(-45.0f);
            break;
        case MIDDLE_HIT:
            if (m_velocity.x > 0)
            {
                m_velocity.SetRotation(-65.0f);
            }
            else
            {
                m_velocity.SetRotation(-120.0f);
            }
            break;

        case LEFT_LOW_HIT:
            m_velocity.SouthWest();
            break;

        case RIGHT_LOW_HIT:
            m_velocity.SouthEast();
            break;
        }

        if (response != ECollisionResult::NO_HIT)
        {
            // Solve collision penetration
            Rect<float> m_shipTransform;
            ship->GetTransform(&m_shipTransform);

            Vec2D dir = m_velocity.GetNormalized();
            Rect<float> temp(*px, *py, m_transform.w, m_transform.h);
            float solveSpeed = 1.0f;
            int pass = 100;
            while (Engine::CheckRects(temp, m_shipTransform) && pass > 0)
            {
                temp.x += dir.x * solveSpeed;
                temp.y += dir.y * solveSpeed;
                pass--;
            }

            *px = temp.x;
            *py = temp.y;
            PlayHitShipSFX();
            return true;
        }
    }

    return false;
}

bool Ball::CheckCollisionWithGrid(float* px, float* py)
{
    int hitIndex = 0;

    // if there is a collision with blocks in the grid, this function
    // returns the closests id using the distance between both rectangle's
    // centers. The ball can hit only one target at a time:
    if (World::Get().CheckCollision(*px, *py, m_transform.w, m_transform.h, &hitIndex))
    {
        *px = m_transform.x;
        *py = m_transform.y;

#if USE_SOUNDFX
        Engine::PlaySFX(m_brickHitSound);
#endif

        int hit = World::Get().HitTile(hitIndex);
        PlayHitBlockSfx(hit);

        int worldX, worldY;
        World::Get().GetWorldPositionFromIndex(hitIndex, &worldX, &worldY);

        bool bottom = (worldY < m_transform.y + m_transform.h);
        bool top = (worldY + World::Get().GetCellHeight() > m_transform.y);
        bool right = (worldX <= m_transform.x + m_transform.w);
        bool left = (worldX + World::Get().GetCellWidth() > m_transform.x);

        // TODO (REVIEW) : see CheckCollisionWithRect... 

        // The ball hits the top but not the bottom or
        // The ball hits the bottom but not the top (same goes for left and right)
        // |   T   |   B   |  T ^ B  |
        // |-------|-------|---------|
        // |   0   |   0   |    0    |
        // |   0   |   1   |    1    |
        // |   1   |   0   |    1    |
        // |   1   |   1   |    0    |
        if (top ^ bottom)
        {
            m_velocity.CW();
        }

        if (left ^ right)
        {
            m_velocity.CCW();
        }

        return true;
    }

    return false;
}

void Ball::PlayHitShipSFX()
{
    static CStopwatch hitSFXStopwatch;
    static bool firstTime = true;

    hitSFXStopwatch.Stop();
    double elapsed = hitSFXStopwatch.ElapsedMilliseconds();
    if (elapsed > SFX_DELAY || firstTime)
    {
        Engine::PlaySFX(m_shipCollisionSfx);
        hitSFXStopwatch.Start();
        firstTime = false;
    }
    else
    {
        LOG(LL_DEBUG, "Multi SFX prevented");
    }
}

void Ball::PlayHitBlockSfx(int hit)
{
    static CStopwatch hitSFXStopwatch;
    static bool firstTime = true;

    hitSFXStopwatch.Stop();
    double elapsed = hitSFXStopwatch.ElapsedMilliseconds();
    if (elapsed > SFX_DELAY || firstTime)
    {
        if (hit > 0)
        {
            Engine::PlaySFX(m_hardBlockCollisionSfx);
        }
        else
        {
            Engine::PlaySFX(m_blockCollisionSfx);
        }

        hitSFXStopwatch.Start();
        firstTime = false;
    }
    else
    {
        LOG(LL_DEBUG, "Multi SFX prevented");
    }
}
