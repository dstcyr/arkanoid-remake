#include "Ship.h"
#include "Ball.h"
#include "maths/MathUtils.h"
#include "Game.h"

Ship::Ship() : Ship(SHIP_START_X, SHIP_START_Y)
{
}

Ship::Ship(float x, float y)
{
    m_transform.x = x;
    m_transform.y = y;
    m_transform.width = 128.0f;
    m_transform.height = 32.0f;
    m_velocity.x = 800.0f;
    m_velocity.y = 0.0f;
    m_vausLeftPart = 0;
    m_vausRightPart = 0;
    m_vausMiddlePart = 0;
    m_laserLeftPart = 0;
    m_laserRightPart = 0;
    m_laserMiddleLeftPart = 0;
    m_laserMiddleRightPart = 0;
    m_sideWidth = 0.0f;
    m_middleWidth = 0.0f;
    m_expandSpeed = 0.01f;
    m_contractTargetWidth = 128.0f;
    m_expandTargetWidth = 192.0f;
    m_LaserActivated = false;
    m_expandSfx = 0;
    m_Dead = false;
}

void Ship::Initialize()
{
    auto& graphics = Game::Get().Graphics();
    auto& audio = Game::Get().Audio();

    m_sideWidth = 32.0f;
    m_middleWidth = 32.0f * 2.0f;
    m_vausLeftPart = graphics.LoadTexture("Images/vausleft.png");
    m_vausRightPart = graphics.LoadTexture("Images/VausRight.png");
    m_vausMiddlePart = graphics.LoadTexture("Images/VausMiddle.png");
    m_laserLeftPart = graphics.LoadTexture("Images/laserLeft.png");
    m_laserRightPart = graphics.LoadTexture("Images/laserRight.png");
    m_laserMiddleLeftPart = graphics.LoadTexture("Images/laserMiddleLeft.png");
    m_laserMiddleRightPart = graphics.LoadTexture("Images/laserMiddleRight.png");
    m_expandSfx = audio.LoadAudio("Audio/expand.wav");
    m_LaserActivated = false;
    m_Dead = false;

    m_playerDeathAnim.Load("Images/death.png");
    m_playerDeathAnim.Init(4, 512, 256);
    m_playerDeathAnim.AddClip("death", 0, 4, 0.1f);
}

void Ship::Update(float dt)
{
    if (m_Dead)
    {
        m_playerDeathAnim.Update(dt);
    }
    else
    {
        float px;
        UpdateControls(dt, &px);
        CheckCollisionWithBounds(&px);

        m_transform.x = px;
        m_sequence.UpdateSequence(dt);

        ProcessShipInput();
    }
}

void Ship::Render()
{
    if (m_Dead)
    {
        m_playerDeathAnim.Draw({m_transform.x - 64, m_transform.y - 64, 256, 128});
    }
    else
    {
        float diff = 32.0f - m_sideWidth;
        if (m_LaserActivated)
        {

            RenderShipPart(m_laserLeftPart, m_transform.x + diff, m_transform.y, m_sideWidth, m_transform.height);
            RenderShipPart(m_laserMiddleLeftPart, m_transform.x + 32.0f, m_transform.y, 32.0f, m_transform.height);
            RenderShipPart(m_laserMiddleRightPart, m_transform.x + 32.0f + 32.0f, m_transform.y, 32.0f, m_transform.height);
            RenderShipPart(m_laserRightPart, m_transform.x + 32.0f + m_middleWidth, m_transform.y, m_sideWidth, m_transform.height);
        }
        else
        {
            RenderShipPart(m_vausLeftPart, m_transform.x + diff, m_transform.y, m_sideWidth, m_transform.height);
            RenderShipPart(m_vausMiddlePart, m_transform.x + 32.0f, m_transform.y, m_middleWidth, m_transform.height);
            RenderShipPart(m_vausRightPart, m_transform.x + 32.0f + m_middleWidth, m_transform.y, m_sideWidth, m_transform.height);
        }

#if SHOW_PADDLE_DEBUG
        Engine::DrawRect(m_transform.x, m_transform.y, m_transform.w, m_transform.h, NColor::ReddishBrown);
        Engine::DrawRect(m_transform.x, m_transform.y, m_transform.w, m_transform.h, NColor(0, 255, 0, 255));

        float halfYheight = 30.0f;
        float leftZoneAStart, leftZoneAEnd, rightZoneAStart, rightZoneAEnd;
        GetZoneDelimiters(&leftZoneAStart, &leftZoneAEnd, &rightZoneAStart, &rightZoneAEnd);

        Engine::DrawLine(leftZoneAStart, m_transform.y - halfYheight, leftZoneAStart, m_transform.y + halfYheight, NColor(0, 255, 0, 255));
        Engine::DrawLine(leftZoneAEnd, m_transform.y - halfYheight, leftZoneAEnd, m_transform.y + halfYheight, NColor(0, 255, 0, 255));
        Engine::DrawLine(rightZoneAStart, m_transform.y - halfYheight, rightZoneAStart, m_transform.y + halfYheight, NColor(0, 255, 0, 255));
        Engine::DrawLine(rightZoneAEnd, m_transform.y - halfYheight, rightZoneAEnd, m_transform.y + halfYheight, NColor(0, 255, 0, 255));

        float halfH = m_transform.h / 2.0f;
        float topLimit = m_transform.y + halfH;
        Engine::DrawLine(m_transform.x - 30.0f, topLimit, m_transform.x + m_transform.w + 30.0f, topLimit, NColor(0, 255, 0, 255));
#endif

    }
}

ECollisionResult Ship::GetCollisionResponse(float x, float y)
{
    float leftZoneAStart, leftZoneAEnd, rightZoneAStart, rightZoneAEnd;
    GetZoneDelimiters(&leftZoneAStart, &leftZoneAEnd, &rightZoneAStart, &rightZoneAEnd);

    float halfH = m_transform.height / 2.0f;
    float topLimit = m_transform.y + halfH;

    if (y < topLimit)
    {
        if (x > leftZoneAStart && x < leftZoneAEnd)
        {
            return ECollisionResult::LEFT_ZONE_HIT;
        }
        else if (x > rightZoneAStart && x < rightZoneAEnd)
        {
            return ECollisionResult::RIGHT_ZONE_HIT;
        }
        else if (x >= leftZoneAEnd && x <= rightZoneAStart)
        {
            return ECollisionResult::MIDDLE_HIT;
        }
        else if (x < leftZoneAStart)
        {
            return ECollisionResult::LEFT_SIDE_HIT;
        }
        else if (x > rightZoneAEnd)
        {
            return ECollisionResult::RIGHT_SIDE_HIT;
        }
    }
    else
    {
        if (x < m_transform.x)
        {
            BX_LOG(ELogLevel::Error, "TOO LOW LEFT");
            return ECollisionResult::LEFT_LOW_HIT;
        }

        BX_LOG(ELogLevel::Error, "TOO LOW RIGHT");
        return ECollisionResult::RIGHT_LOW_HIT;
    }

    return ECollisionResult::NO_HIT;
}

void Ship::UpdateControls(float dt, float* px)
{
    auto& inputs = Game::Get().Inputs();

    static bool movePressed = false;
    static float translationElapsed = 0.0f;
    static const float translationDelay = 0.0f;
    *px = m_transform.x;

    if (inputs.IsKeyDown(EKey::EKEY_LEFT))
    {
        if (!movePressed)
        {
            movePressed = true;
            *px = m_transform.x - m_velocity.x * dt;
            translationElapsed = 0.0f;
        }
        else
        {
            translationElapsed += dt;
            if (translationElapsed >= translationDelay)
            {
                translationElapsed = 0.0f;
                *px = m_transform.x - m_velocity.x * dt;
            }
        }
    }
    else if (inputs.IsKeyDown(EKey::EKEY_RIGHT))
    {
        if (!movePressed)
        {
            movePressed = true;
            *px = m_transform.x + m_velocity.x * dt;
            translationElapsed = 0.0f;
        }
        else
        {
            translationElapsed += dt;
            if (translationElapsed >= translationDelay)
            {
                translationElapsed = 0.0f;
                *px = m_transform.x + m_velocity.x * dt;
            }
        }
    }
    else if (movePressed)
    {
        movePressed = false;
    }
}

void Ship::CheckCollisionWithBounds(float* px)
{
    auto& world = Game::Get().World();

    float rightLimit = RIGHT_WALL_X;

    // TODO (DaSt)
    // IState* currentState = world.GetTopState();
    // if (currentState)
    // {
    //     Game* gameInstance = dynamic_cast<Game*>(currentState);
    //     if (gameInstance && gameInstance->WarpDoorOpenned())
    //     {
    //         if (*px + 32.0f > RIGHT_WALL_X)
    //         {
    //             *px = SCREEN_WIDTH;
    //             gameInstance->OnWarpedOut();
    //         }
    //     }
    //     else if (*px > RIGHT_WALL_X - m_transform.w)
    //     {
    //         *px = RIGHT_WALL_X - m_transform.w;
    //     }
    // 
    //     if (*px < LEFT_WALL_X)
    //     {
    //         *px = LEFT_WALL_X;
    //     }
    // }
}

void Ship::GetZoneDelimiters(float* startA, float* endA, float* startB, float* endB)
{
    *startA = m_transform.x + SIDE_WIDTH;
    *endA = *startA + 32.0f - 8.0f;
    *startB = m_transform.x + 32.0f + m_middleWidth;
    *endB = *startB + 32.0f - 8.0f;
}

bool Ship::TaskExpandShip(float dt, ShipState* state)
{
    auto& audio = Game::Get().Audio();

    static bool firstTime = true;

    if (state->firstPass)
    {
        audio.PlaySFX(m_expandSfx);
        state->firstPass = false;
    }

    state->elapsed += dt;

    if (state->elapsed > m_expandSpeed)
    {
        state->elapsed = 0.0f;
        m_transform.x -= 2.0f;
        m_transform.width += 4.0f;
        if (m_transform.width >= m_expandTargetWidth)
        {
            m_transform.width = m_expandTargetWidth;
            m_middleWidth = m_transform.width - 32.0f - 32.0f;
            return false;
        }
    }

    m_middleWidth = m_transform.width - 32.0f - 32.0f;
    return true;
}

bool Ship::TaskContractShip(float dt, ShipState* state)
{
    state->elapsed += dt;

    if (state->elapsed > m_expandSpeed)
    {
        state->elapsed = 0.0f;
        m_transform.x += 2.0f;
        m_transform.width -= 4.0f;
        if (m_transform.width <= m_contractTargetWidth)
        {
            m_transform.width = m_contractTargetWidth;
            m_middleWidth = m_transform.width - 32.0f - 32.0f;
            return false;
        }
    }

    m_middleWidth = m_transform.width - 32.0f - 32.0f;
    return true;
}

bool Ship::TaskActivateLaser(float dt, ShipState* state)
{
    state->elapsed += dt;
    if (state->elapsed > m_expandSpeed)
    {
        state->elapsed = 0.0f;

        switch (state->phase)
        {

        case 0:
            m_sideWidth = MathUtils::Clamp(m_sideWidth - 2.0f, 0.0f, SIDE_ZONE_WIDTH);
            if (m_sideWidth <= 0.0f)
            {
                m_sideWidth = 0.0f;
                m_LaserActivated = true;
                state->phase = 1;
            }
            break;

        case 1:
            m_sideWidth = MathUtils::Clamp(m_sideWidth + 2.0f, 0.0f, SIDE_ZONE_WIDTH);
            if (m_sideWidth >= SIDE_ZONE_WIDTH)
            {
                m_sideWidth = SIDE_ZONE_WIDTH;
                return false;
            }
            break;
        }
    }

    return true;
}

bool Ship::TaskDeactivateLaser(float dt, ShipState* state)
{
    state->elapsed += dt;
    if (state->elapsed > m_expandSpeed)
    {
        state->elapsed = 0.0f;

        switch (state->phase)
        {

        case 0:
            m_sideWidth = MathUtils::Clamp(m_sideWidth - 2.0f, 0.0f, SIDE_ZONE_WIDTH);
            if (m_sideWidth <= 0.0f)
            {
                m_sideWidth = 0.0f;
                m_LaserActivated = false;
                state->phase = 1;
            }
            break;

        case 1:
            m_sideWidth = MathUtils::Clamp(m_sideWidth + 2.0f, 0.0f, SIDE_ZONE_WIDTH);
            if (m_sideWidth >= SIDE_ZONE_WIDTH)
            {
                m_sideWidth = SIDE_ZONE_WIDTH;
                return false;
            }
            break;
        }
    }

    return true;
}

void Ship::ProcessShipInput()
{
    auto& inputs = Game::Get().Inputs();

    if (inputs.IsKeyDown(EKey::EKEY_A))
    {
        if (m_LaserActivated)
        {
            float leftSx = m_transform.x + (SIDE_ZONE_WIDTH / 2.0f) - (LASER_WIDTH / 2.0f);
            float rightSx = m_transform.x + m_transform.width - SIDE_ZONE_WIDTH + (SIDE_ZONE_WIDTH / 2.0f) - (LASER_WIDTH / 2.0f);
            OnLaserShotDelegate.Invoke<LaserEvent>(leftSx, m_transform.y, rightSx, m_transform.y);
        }
        // else if (m_balls.size() > 0)
        // {
        //     for (Ball* stuckBall : m_balls)
        //     {
        //         stuckBall->SetSpeed(500.0f);
        //         stuckBall->SetAngle(SHARP_ANGLE);
        //         stuckBall->ChangeDirection(0, -1);
        //     }
        // 
        //     m_balls.clear();
        // }
    }
}

void Ship::RenderShipPart(size_t id, float x, float y, float w, float h)
{
    auto& graphics = Game::Get().Graphics();

    if (x < RIGHT_WALL_X)
    {
        graphics.DrawImage(id, { x, y, w, h }, 255, 255, 255, 255);
    }
}

void Ship::ExpandShip()
{
    BX_LOG(ELogLevel::Log, "Expand ship");
    m_sequence.Add(this, &Ship::TaskExpandShip);
}

void Ship::ContractShip()
{
    BX_LOG(ELogLevel::Log, "Contact ship");
    m_sequence.Add(this, &Ship::TaskContractShip);
}

void Ship::ActivateLaser()
{
    m_sequence.Add(this, &Ship::TaskActivateLaser);
}

void Ship::DeactivateLaser()
{
    m_sequence.Add(this, &Ship::TaskDeactivateLaser);
}

void Ship::Die()
{
    m_Dead = true;
    m_playerDeathAnim.Play("death", false);
}
