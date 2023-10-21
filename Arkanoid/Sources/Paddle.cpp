#include "Paddle.h"
#include "Engine.h"
#include "Log.h"
#include "Ball.h"
#include "Collision.h"
#include "MathUtils.h"

#if SHOW_PADDLE_DEBUG_COLLISION
float cpX = 0.0f;
float cpY = 0.0f;
#endif

Paddle::Paddle()
{
    m_transform = Rect<float>(0.0f, 0.0f, 0.0f, 0.0f);
    m_translationElapsed = 0.0f;
    m_translationDelay = 0.0f;
    m_velocity = 0.0f;
    m_vausLeftPart = 0;
    m_vausRightPart = 0;
    m_vausMiddlePart = 0;
    m_laserLeftPart = 0;
    m_laserRightPart = 0;
    m_laserMiddleLeftPart = 0;
    m_laserMiddleRightPart = 0;
    m_hitSound = 0;
    m_expandElapsed = 0.0f;
    m_expandSpeed = 0.01f;
    m_contractTargetWidth = 128.0f;
    m_expandTargetWidth = 192.0f;
    m_armingElapsed = 0.0f;
    m_sideWidth = 0.0f;
    m_armed = false;
    m_sticky = false;
}

void Paddle::Initialize()
{
    m_transform.x = 400.0f;
    m_transform.y = 860.0f;
    m_transform.w = 128.0f;
    m_transform.h = 32.0f;
    m_velocity = 800.0f;
    m_expandElapsed = 0.0f;
    m_expandSpeed = 0.01f;
    m_contractTargetWidth = 128.0f;
    m_expandTargetWidth = 192.0f;
    m_armingElapsed = 0.0f;
    m_sideWidth = SIDE_ZONE_WIDTH;
    m_armed = false;
    m_sticky = false;

    m_vausLeftPart = Engine::LoadTexture("Assets/Images/VausLeft.png");
    m_vausRightPart = Engine::LoadTexture("Assets/Images/VausRight.png");
    m_vausMiddlePart = Engine::LoadTexture("Assets/Images/VausMiddle.png");
    m_laserLeftPart = Engine::LoadTexture("Assets/Images/laserLeft.png");
    m_laserRightPart = Engine::LoadTexture("Assets/Images/laserRight.png");
    m_laserMiddleLeftPart = Engine::LoadTexture("Assets/Images/laserMiddleLeft.png");
    m_laserMiddleRightPart = Engine::LoadTexture("Assets/Images/laserMiddleRight.png");
    m_hitSound = Engine::LoadSound("Assets/Audio/PaddleHit.wav");
}

void Paddle::Update(float dt, float minX, float maxX)
{
    float pX;
    UpdateControls(dt, &pX);

#if TOGGLE_PADDLE_DEBUG
    if (Engine::GetKeyDown(EKey::KEY_DEBUG))
    {
        // TEST:
    }
#endif

    if (pX < minX)
    {
        pX = minX;
    }
    else if (pX > maxX - m_transform.w)
    {
        pX = maxX - m_transform.w;
    }
    
    m_transform.x = pX;

    for (Ball* ball : m_balls)
    {
        float diff = pX - m_transform.x;
        Rect<float> ballTransform;
        ball->GetTransform(&ballTransform);
        //ballTransform.x += m_transform.x + diff;

        ball->SetPosition(ballTransform.x + diff, ballTransform.y);
    }

    if (Engine::GetKeyDown(KEY_A))
    {
        if (m_armed)
        {
            float leftSx = m_transform.x + (SIDE_ZONE_WIDTH / 2.0f) - (LASER_WIDTH / 2.0f);
            float rightSx = m_transform.x + m_transform.w - SIDE_ZONE_WIDTH + (SIDE_ZONE_WIDTH / 2.0f) - (LASER_WIDTH / 2.0f);
            OnLaserShotDelegate.Invoke<LaserEvent>(leftSx, m_transform.y, rightSx, m_transform.y, LASER_SPEED);
        }
        else if (m_balls.size() > 0)
        {
            for (Ball* stuckBall : m_balls)
            {
                stuckBall->SetSpeed(500.0f);
                stuckBall->SetAngle(SHARP_ANGLE);
                stuckBall->ChangeDirection(0, -1);
            }

            m_balls.clear();
        }
    }

    if (m_transform.x > RIGHT_WALL_X)
    {
        OnExitLevelDelegate.Invoke<PaddleEvent>();
    }
}

void Paddle::Render()
{
    if (m_armed)
    {
        float diff = 32 - m_sideWidth;

        float xx = m_transform.x + diff + diff;
        Engine::DrawTexture(m_laserLeftPart, xx, m_transform.y, m_sideWidth, m_transform.h);

        xx = m_transform.x + 32 + 64 - diff;
        if (xx < RIGHT_WALL_X)
        {
            Engine::DrawTexture(m_laserRightPart, xx, m_transform.y, m_sideWidth, m_transform.h);
        }

        xx = m_transform.x + 32.0f;
        if (xx < RIGHT_WALL_X)
        {
            Engine::DrawTexture(m_laserMiddleLeftPart, xx, m_transform.y, 32.0f, m_transform.h);
        }

        xx = m_transform.x + 64.0f;
        if (xx < RIGHT_WALL_X)
        {
            Engine::DrawTexture(m_laserMiddleRightPart, xx, m_transform.y, 32.0f, m_transform.h);
        }
    }
    else
    {
        float diff = 32 - m_sideWidth;
        float middleWidth = m_transform.w - 32 - 32;

        float xx = m_transform.x + diff + diff;

        if (xx < RIGHT_WALL_X)
        {
            Engine::DrawTexture(m_vausLeftPart, m_transform.x + diff + diff, m_transform.y, m_sideWidth, m_transform.h);
        }

        xx = m_transform.x + 32 + middleWidth - diff;
        if (xx < RIGHT_WALL_X)
        {
            Engine::DrawTexture(m_vausRightPart, m_transform.x + 32 + middleWidth - diff, m_transform.y, m_sideWidth, 32.0f);
        }

        xx = m_transform.x + 32.0f;
        if (xx < RIGHT_WALL_X)
        {
            Engine::DrawTexture(m_vausMiddlePart, m_transform.x + 32.0f, m_transform.y, middleWidth, 32.0f);
        }
    }

#if SHOW_PADDLE_DEBUG_ZONES
    float x1 = m_transform.x;
    float y1 = m_transform.y - 30.0f;
    float x2 = m_transform.x;
    float y2 = m_transform.y + 60.0f;
    Engine::DrawLine(x1, y1, x2, y2, NColor::ReddishBrown);

    float x3 = m_transform.x + SIDE_ZONE_WIDTH;
    float y3 = m_transform.y - 30.0f;
    float x4 = m_transform.x + SIDE_ZONE_WIDTH;
    float y4 = m_transform.y + 60.0f;
    Engine::DrawLine(x3, y3, x4, y4, NColor::ReddishBrown);

    Engine::FillRect(x1, m_transform.y, x3 - x1, m_transform.h, NColor::Red);

    x1 = m_transform.x + m_transform.w;
    y1 = m_transform.y - 30.0f;
    x2 = m_transform.x + m_transform.w;
    y2 = m_transform.y + 60.0f;
    Engine::DrawLine(x1, y1, x2, y2, NColor::ReddishBrown);

    x3 = m_transform.x + m_transform.w - SIDE_ZONE_WIDTH;
    y3 = m_transform.y - 30.0f;
    x4 = m_transform.x + m_transform.w - SIDE_ZONE_WIDTH;
    y4 = m_transform.y + 60.0f;
    Engine::DrawLine(x3, y3, x4, y4, NColor::ReddishBrown);

    Engine::FillRect(x1, m_transform.y, x3 - x1, m_transform.h, NColor::Red);
#endif

#if SHOW_PADDLE_DEBUG
    Engine::DrawRect(m_transform.x, m_transform.y, m_transform.w, m_transform.h, NColor::ReddishBrown);
#endif

#if SHOW_PADDLE_DEBUG_COLLISION
    Engine::DrawCircle(m_transform.x + cpX, m_transform.y + cpY, 10.0f, NColor::DarkForestGreen);
#endif
}

void Paddle::CheckCollisionWith(Ball* ball)
{
    // Where the ball hits the paddle dictates how it will bounce off the paddle.
    // 
    // - If the ball hits the area in the middle, it will bounce off at a sharp angle.
    // - If the ball hits the areas near the sides, it will bounce off at a steeper angle.
    // - If the ball hits the very edges of the paddle, it will bounce off at a very shallow angle.
    // 
    // Use this information to not only keep the ball in play, but to better direct the
    // path that it takes while you attempt to knock out all of the bricks.
    CHECK(ball);

    Rect<float> ballTransform;
    ball->GetTransform(&ballTransform);
    float hcw = ballTransform.w / 2.0f;
    float hch = ballTransform.h / 2.0f;
    float cx = ballTransform.x + hcw;
    float cy = ballTransform.y + hch;

    float hitX, hitY;
    if (Engine::CheckRectCircle(m_transform.x, m_transform.y, m_transform.w, m_transform.h, cx, cy, hcw, &hitX, &hitY))
    {
        if (m_sticky)
        {
            bool alreadyIn = false;
            for (Ball* stuckBall : m_balls)
            {
                // TODO (REVIEW): No capsule can spawn when there is more than one ball. No need to use a list
                if (stuckBall == ball)
                {
                    alreadyIn = true;
                    break;
                }
            }

            if (!alreadyIn)
            {
                m_balls.push_back(ball);
                ball->SetSpeed(0.0f);
            }
        }
        else
        {
            Vec2D ballVelocity;
            ball->GetVelocity(&ballVelocity);

#if USE_SOUNDFX
            Engine::PlaySFX(m_hitSound, 1, 0);
#endif

#if SHOW_PADDLE_DEBUG_COLLISION
            cpX = hitX - m_transform.x;
            cpY = hitY - m_transform.y;
#endif
            float leftZoneAStart = m_transform.x;
            float leftZoneAEnd = leftZoneAStart + SIDE_ZONE_WIDTH;
            float rightZoneAStart = m_transform.x + m_transform.w - SIDE_ZONE_WIDTH;
            float rightZoneAEnd = m_transform.x + m_transform.w;

            int dirY = -1;
            if (ballTransform.y > m_transform.y)
            {
                dirY = 1;
            }

            if (cx > leftZoneAEnd && cx < rightZoneAStart)
            {
                int dirX = 1;
                if (ballVelocity.x < 0)
                {
                    dirX = -1;
                }

                ball->SetAngle(STEEP_ANGLE);
                ball->ChangeDirection(dirX, dirY);
            }

            if (cx >= leftZoneAStart && cx <= leftZoneAEnd)
            {
                ball->SetAngle(SHARP_ANGLE);
                ball->ChangeDirection(-1, dirY);
            }

            if (cx >= rightZoneAStart && cx <= rightZoneAEnd)
            {
                ball->SetAngle(SHARP_ANGLE);
                ball->ChangeDirection(1, dirY);
            }
        }
    }
}

void Paddle::GetTransform(Rect<float>* transform)
{
    *transform = m_transform;
}

void Paddle::SetSticky(bool activate)
{
    m_sticky = activate;

    if (!m_sticky)
    {
        for (Ball* ball : m_balls)
        {
            // relauched balls ?
        }
        m_balls.clear();
    }
}

void Paddle::UpdateControls(float dt, float* px)
{
    static bool movePressed = false;

    *px = m_transform.x;

    if (Engine::GetKey(KEY_LEFT))
    {
        if (!movePressed)
        {
            movePressed = true;
            *px = m_transform.x - m_velocity * dt;
            m_translationElapsed = 0.0f;
        }
        else
        {
            m_translationElapsed += dt;
            if (m_translationElapsed >= m_translationDelay)
            {
                m_translationElapsed = 0.0f;
                *px = m_transform.x - m_velocity * dt;
            }
        }
    }
    else if (Engine::GetKey(KEY_RIGHT))
    {
        if (!movePressed)
        {
            movePressed = true;
            *px = m_transform.x + m_velocity * dt;
            m_translationElapsed = 0.0f;
        }
        else
        {
            m_translationElapsed += dt;
            if (m_translationElapsed >= m_translationDelay)
            {
                m_translationElapsed = 0.0f;
                *px = m_transform.x + m_velocity * dt;
            }
        }
    }
    else if (movePressed)
    {
        movePressed = false;
    }
}

bool Paddle::TaskExpandShip(float dt)
{
    m_expandElapsed += dt;

    if (m_expandElapsed > m_expandSpeed)
    {
        m_expandElapsed = 0.0f;
        m_transform.x -= 2.0f;
        m_transform.w += 4.0f;
        if (m_transform.w >= m_expandTargetWidth)
        {
            m_transform.w = m_expandTargetWidth;
            return false;
        }
    }

    return true;
}

bool Paddle::TaskContractShip(float dt)
{
    m_expandElapsed += dt;

    if (m_expandElapsed > m_expandSpeed)
    {
        m_expandElapsed = 0.0f;
        m_transform.x += 2.0f;
        m_transform.w -= 4.0f;
        if (m_transform.w <= m_contractTargetWidth)
        {
            m_transform.w = m_contractTargetWidth;
            return false;
        }
    }

    return true;
}

bool Paddle::TaskActivateLaser(float dt)
{
    return UpdateLaserTask(dt, true);
}

bool Paddle::TaskDeactivateLaser(float dt)
{
    return UpdateLaserTask(dt, false);
}

bool Paddle::UpdateLaserTask(float dt, bool activate)
{
    static int activationPhase = 0;

    m_armingElapsed += dt;
    if (m_armingElapsed > m_expandSpeed)
    {
        m_armingElapsed = 0.0f;

        switch (activationPhase)
        {

        case 0:
            m_sideWidth = Engine::Clamp(m_sideWidth - 2.0f, 0.0f, SIDE_ZONE_WIDTH);
            if (m_sideWidth <= 0.0f)
            {
                m_sideWidth = 0.0f;
                m_armed = activate;
                activationPhase++;
            }
            break;

        case 1:
            m_sideWidth = Engine::Clamp(m_sideWidth + 2.0f, 0.0f, SIDE_ZONE_WIDTH);
            if (m_sideWidth >= SIDE_ZONE_WIDTH)
            {
                m_sideWidth = SIDE_ZONE_WIDTH;
                activationPhase = 0;
                return false;
            }
            break;
        }
    }

    return true;
}
