#include "BallManager.h"
#include "World.h"
#include "Ship.h"
#include "Stopwatch.h"
#include "Game.h"
#include "utils/Checks.h"

void BallManager::Initialize()
{
    auto& graphics = Game::Get().Graphics();
    auto& audio = Game::Get().Audio();

    m_ballTexture = graphics.LoadTexture("Images/Ball.png");
    m_shipCollisionSfx = audio.LoadAudio("Audio/shiphit.wav");
    audio.SetVolume(m_shipCollisionSfx, 50);
    m_blockCollisionSfx = audio.LoadAudio("Audio/blockhit.wav");
    audio.SetVolume(m_blockCollisionSfx, 50);
    m_hardBlockCollisionSfx = audio.LoadAudio("Audio/hardblockhit.wav");
    audio.SetVolume(m_hardBlockCollisionSfx, 50);

    m_catchingBalls = false;
    m_firstBall = false;
    m_firstBallElapsed = 0.0f;
}

void BallManager::SpawnBall(int count)
{
    float x = BALL_START_X;
    float y = BALL_START_Y;
    float angle = -65.0f;

    size_t len = m_activeBalls.size();
    if (len > 0)
    {
        Ball* ball = m_activeBalls[len - 1];
        x = ball->GetX();
        y = ball->GetY();
        angle = ball->GetAngle() - 45.0f;
    }

    for (int i = 0; i < count; i++)
    {
        Ball* newBall = new Ball(x, y);
        newBall->SetAngle(angle);
        newBall->SetSpeed(m_gameSpeed);
        m_activeBalls.push_back(newBall);

        angle -= 45.0f;
    }
}

void BallManager::Update(float dt)
{
    if (m_firstBall)
    {
        m_firstBallElapsed += dt;
        if (m_firstBallElapsed > 2)
        {
            m_firstBall = false;
            m_firstBallElapsed = 0.0f;

            if (!m_catchingBalls)
            {
                ForceLaunchCaughtBalls();
            }
        }
    }

    ProcessInput();

    Rect<float> transform;
    Vec2<float> velocity;

    Ship* ship = World::Get().GetShip();
    for (CatchInfo& info : m_caughtBalls)
    {
        BX_CHECKS(ship, "Invalid ship");
        info.ball->SetPosition(
            ship->GetX() - info.offsetX,
            ship->GetY() - info.offsetY
        );
    }

    for (Ball* ball : m_activeBalls)
    {
        BX_CHECKS(ball, "Invalid ball");
        ball->GetTransform(&transform);
        ball->GetVelocity(&velocity);

        // prediction for the new position:
        float pX = transform.x + velocity.x * dt;
        float pY = transform.y + velocity.y * dt;

        // check bounds collision and update the predicted position:
        ECollisionBoundType collisionBt = CheckCollisionWithBounds(&pX, &pY, transform, velocity);
        if (collisionBt != ECollisionBoundType::NO_COLLISION)
        {
            if (collisionBt == ECollisionBoundType::BOTTOM_COLLISION)
            {
                OnBottomReached.Invoke<BallEvent>(pX, pY);
#if !GOD_MODE
                Remove(ball);
                return;
#endif
            }
        }

        if (ball->CheckCollisionWith(*ship))
        {
            ECollisionResult result = CheckCollisionWithShip(&pX, &pY, ship, transform, velocity);
            if (m_catchingBalls)
            {
                if (result != ECollisionResult::LEFT_LOW_HIT && result != ECollisionResult::RIGHT_LOW_HIT && result != ECollisionResult::NO_HIT)
                {
                    CatchInfo info;
                    info.ball = ball;
                    info.offsetX = ship->GetX() - pX;
                    info.offsetY = ship->GetY() - pY;
                    m_caughtBalls.push_back(info);
                    Remove(ball);
                }
            }
        }

        CheckCollisionWithGrid(&pX, &pY, transform, velocity);
        ball->SetVelocity(velocity);
        ball->SetPosition(pX, pY);
    }

    if (m_gameSpeed < GAME_SPEED)
    {
        m_slowdownElapsed += dt;
        if (m_slowdownElapsed >= SLOWDOWN_DELAY)
        {
            m_slowdownElapsed = 0.0f;
            SetGameSpeed(GAME_SPEED);
        }
    }
}

void BallManager::Render()
{
    auto& graphics = Game::Get().Graphics();

    for (Ball* ball : m_activeBalls)
    {
        BX_CHECKS(ball, "invalid ball");
        Rect<float> transform;
        ball->GetTransform(&transform);
        graphics.DrawImage(m_ballTexture, transform, 255, 255, 255, 255);

#if SHOW_BALL_DEBUG
        Engine::DrawLine(transform.x + transform.w / 2.0f, 0.0f, transform.x + transform.w / 2.0f, 1000.0f, NColor::LightLavender);
        Engine::DrawLine(0.0f, transform.y + transform.h / 2.0f, 1500.0f, transform.y + transform.h / 2.0f, NColor::LightLavender);
        Engine::DrawRect(transform.x, transform.y, transform.w, transform.h, NColor(0, 255, 0, 255));
#endif
    }

    for (CatchInfo& info : m_caughtBalls)
    {
        BX_CHECKS(info.ball, "invalid ball");
        Rect<float> transform;
        info.ball->GetTransform(&transform);
        graphics.DrawImage(m_ballTexture, transform, 255, 255, 255, 255);
    }
}

void BallManager::Clear()
{
    for (Ball* ball : m_activeBalls)
    {
        SAFE_DELETE(ball);
    }

    m_activeBalls.clear();

    OnBottomReached.Clear();
}

int BallManager::Count()
{
    return static_cast<int>(m_activeBalls.size());
}

void BallManager::SlowDown()
{
    SetGameSpeed(m_gameSpeed * SLOWDOWN_ITR);
    m_slowdownElapsed = 0.0f;
}

void BallManager::SetGameSpeed(float speed)
{
    m_gameSpeed = MathUtils::Clamp(speed, 100.0f, GAME_SPEED);
    for (Ball* ball : m_activeBalls)
    {
        ball->SetSpeed(m_gameSpeed);
    }
}

ECollisionBoundType BallManager::CheckCollisionWithBounds(float* px, float* py, Rect<float>& transform, Vec2<float>& velocity)
{
    ECollisionBoundType result = ECollisionBoundType::NO_COLLISION;
    if (*px > RIGHT_WALL_X - transform.width)
    {
        velocity.CCW();
        *px = RIGHT_WALL_X - transform.width;
        result = ECollisionBoundType::WALL_COLLISION;
    }
    else if (*px < LEFT_WALL_X)
    {
        velocity.CCW();
        *px = LEFT_WALL_X;
        result = ECollisionBoundType::WALL_COLLISION;
    }

    if (*py > BOTTOM_WALL_Y - transform.height)
    {
        velocity.CW();
        *py = BOTTOM_WALL_Y - transform.height;
        result = ECollisionBoundType::BOTTOM_COLLISION;
    }
    else if (*py < TOP_WALL_Y)
    {
        velocity.CW();
        *py = TOP_WALL_Y;
        result = ECollisionBoundType::TOP_COLLISION;
    }

    return result;
}

ECollisionResult BallManager::CheckCollisionWithShip(float* px, float* py, Ship* ship, Rect<float>& transform, Vec2<float>& velocity)
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
    BX_CHECKS(ship, "invalid ball");

    float halfWidth = transform.width / 2.0f;
    float halfHeight = transform.height / 2.0f;
    float centerX = transform.x + halfWidth;
    float centerY = transform.y + halfHeight;

    ECollisionResult response = ship->GetCollisionResponse(centerX, centerY);
    switch (response)
    {
    case LEFT_SIDE_HIT:
        velocity.SetRotation(-150.0f);
        break;
    case RIGHT_SIDE_HIT:
        velocity.SetRotation(-30.0f);
        break;
    case LEFT_ZONE_HIT:
        velocity.SetRotation(-135.0f);
        break;
    case RIGHT_ZONE_HIT:
        velocity.SetRotation(-45.0f);
        break;
    case MIDDLE_HIT:
        if (velocity.x > 0)
        {
            velocity.SetRotation(-65.0f);
        }
        else
        {
            velocity.SetRotation(-120.0f);
        }
        break;

    case LEFT_LOW_HIT:
        velocity.SouthWest();
        break;

    case RIGHT_LOW_HIT:
        velocity.SouthEast();
        break;
    }

    if (response != ECollisionResult::NO_HIT)
    {
        // Solve collision penetration
        Rect<float> m_shipTransform;
        ship->GetTransform(&m_shipTransform);

        Vec2<float> dir = velocity.GetNormalized();
        Rect<float> temp(*px, *py, transform.width, transform.height);
        float solveSpeed = 1.0f;
        int pass = 100;
        while (MathUtils::IsColliding(temp, m_shipTransform) && pass > 0)
        {
            temp.x += dir.x * solveSpeed;
            temp.y += dir.y * solveSpeed;
            pass--;
        }

        *px = temp.x;
        *py = temp.y;
        PlayHitShipSFX();
    }

    return response;
}

bool BallManager::CheckCollisionWithGrid(float* px, float* py, Rect<float>& transform, Vec2<float>& velocity)
{
    int hitIndex = 0;

    // if there is a collision with blocks in the grid, this function
    // returns the closests id using the distance between both rectangle's
    // centers. The ball can hit only one target at a time:
    if (World::Get().CheckCollision(*px, *py, transform.width, transform.height, &hitIndex))
    {
        *px = transform.x;
        *py = transform.y;

#if USE_SOUNDFX
        Engine::PlaySFX(m_brickHitSound);
#endif

        int hit = World::Get().HitTile(hitIndex);
        PlayHitBlockSfx(hit);

        int worldX, worldY;
        World::Get().GetWorldPositionFromIndex(hitIndex, &worldX, &worldY);

        bool bottom = (worldY < transform.y + transform.height);
        bool top = (worldY + World::Get().GetCellHeight() > transform.y);
        bool right = (worldX <= transform.x + transform.width);
        bool left = (worldX + World::Get().GetCellWidth() > transform.x);

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
            velocity.CW();
        }

        if (left ^ right)
        {
            velocity.CCW();
        }

        return true;
    }

    return false;
}

void BallManager::Remove(Ball* ball)
{
    auto it = m_activeBalls.begin();
    while (it != m_activeBalls.end())
    {
        if (*it == ball)
        {
            //SAFE_DELETE(ball);
            //m_ballsToDelete.push_back(ball);
            it = m_activeBalls.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void BallManager::CatchingBall(bool catching)
{
    m_catchingBalls = catching;
    if (!m_catchingBalls)
    {
        ForceLaunchCaughtBalls();
    }
}

void BallManager::FirstBallOfTheRound()
{
    SpawnBall(1);

    Ship* ship = World::Get().GetShip();
    BX_CHECKS(ship, "invalid ball");

    for (Ball* ball : m_activeBalls)
    {
        CatchInfo info;
        info.ball = ball;
        info.offsetX = -((ship->GetW() / 2.0f) - (ball->GetW() / 2.0f));
        info.offsetY = ball->GetH();
        m_caughtBalls.push_back(info);
    }

    m_activeBalls.clear();
    m_firstBall = true;
    m_firstBallElapsed = 0.0f;
}

bool BallManager::CheckCollisionWith(MovingObject& other)
{
    for (Ball* ball : m_activeBalls)
    {
        if (ball->CheckCollisionWith(other))
        {
            Rect<float> otherTransform;
            other.GetTransform(&otherTransform);

            Vec2<float> velocity;
            Rect<float> ballTransform;
            float ballX, ballY;
            ball->GetTransform(&ballTransform);
            ballTransform.GetCenter(&ballX, &ballY);
            ball->GetVelocity(&velocity);

            bool bottom = ballY > otherTransform.y + otherTransform.height;
            bool top = ballY < otherTransform.y;
            bool left = ballX < otherTransform.x;
            bool right = ballX > otherTransform.x + otherTransform.width;

            if (top ^ bottom)
            {
                velocity.CW();
            }

            if (left ^ right)
            {
                velocity.CCW();
            }

            ball->SetVelocity(velocity);
            return true;
        }
    }

    return false;
}

void BallManager::PlayHitShipSFX()
{
    auto& audio = Game::Get().Audio();

    static CStopwatch hitSFXStopwatch;
    static bool firstTime = true;

    hitSFXStopwatch.Stop();
    double elapsed = hitSFXStopwatch.ElapsedMilliseconds();
    if (elapsed > SFX_DELAY || firstTime)
    {
        audio.PlaySFX(m_shipCollisionSfx);
        hitSFXStopwatch.Start();
        firstTime = false;
    }
    else
    {
        BX_LOG(ELogLevel::Log, "Multi SFX prevented");
    }
}

void BallManager::ProcessInput()
{
    auto& input = Game::Get().Inputs();

    if (input.IsKeyDown(EKey::EKEY_A))
    {
        if (m_caughtBalls.size() > 0)
        {
            for (CatchInfo& info : m_caughtBalls)
            {
                m_activeBalls.push_back(info.ball);
            }

            m_caughtBalls.clear();
        }
    }
}

void BallManager::ForceLaunchCaughtBalls()
{
    for (CatchInfo& info : m_caughtBalls)
    {
        m_activeBalls.push_back(info.ball);
    }

    m_caughtBalls.clear();
}

void BallManager::PlayHitBlockSfx(int hit)
{
    auto& audio = Game::Get().Audio();

    static CStopwatch hitSFXStopwatch;
    static bool firstTime = true;

    hitSFXStopwatch.Stop();
    double elapsed = hitSFXStopwatch.ElapsedMilliseconds();
    if (elapsed > SFX_DELAY || firstTime)
    {
        if (hit > 0)
        {
            audio.PlaySFX(m_hardBlockCollisionSfx);
        }
        else
        {
            audio.PlaySFX(m_blockCollisionSfx);
        }

        hitSFXStopwatch.Start();
        firstTime = false;
    }
    else
    {
        BX_LOG(ELogLevel::Log, "Multi SFX prevented");
    }
}