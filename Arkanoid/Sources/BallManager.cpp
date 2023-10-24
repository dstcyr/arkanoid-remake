#include "BallManager.h"
#include "Log.h"
#include "MathUtils.h"

void BallManager::SpawnBall(int count)
{
    float x = BALL_START_X;
    float y = BALL_START_Y;
    float angle = -65.0f;

    size_t len = m_activeBalls.size();
    if(len > 0)
    {
        Ball* ball = m_activeBalls[len - 1];
        x = ball->GetX();
        y = ball->GetY();
        angle = ball->GetAngle() - 45.0f;
    }

    for (int i = 0; i < count; i++)
    {
        Ball* newBall = new Ball(x, y);
        newBall->Initialize();
        newBall->SetAngle(angle);
        newBall->SetSpeed(m_gameSpeed);
        newBall->OnBottomReached.Bind(this, &BallManager::OnBallReachedBottomCallback);
        m_activeBalls.push_back(newBall);

        angle -= 45.0f;
    }
}

void BallManager::Update(float dt)
{
    for (Ball* ball : m_activeBalls)
    {
        CHECK(ball);
        ball->Update(dt);
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
    for (Ball* ball : m_activeBalls)
    {
        CHECK(ball);
        ball->Render();
    }
}

void BallManager::Destroy()
{
    for (Ball* ball : m_activeBalls)
    {
        ball->OnBottomReached.Clear();
        SAFE_DELETE(ball);
    }

    m_activeBalls.clear();
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

void BallManager::OnBallReachedBottomCallback(const BallEvent& ballEvent)
{
    OnBallReachedBottom.Invoke<BallEvent>(ballEvent.ball, ballEvent.x, ballEvent.y);
}

void BallManager::SetGameSpeed(float speed)
{
    m_gameSpeed = Engine::Clamp(speed, 100.0f, GAME_SPEED);
    for (Ball* ball : m_activeBalls)
    {
        ball->SetSpeed(m_gameSpeed);
    }
}

void BallManager::Clear()
{
    for (Ball* ball : m_activeBalls)
    {
        SAFE_DELETE(ball);
    }

    m_activeBalls.clear();
}

void BallManager::Remove(Ball* ball)
{
    auto it = m_activeBalls.begin();
    while (it != m_activeBalls.end())
    {
        if (*it == ball)
        {
            //SAFE_DELETE(ball);
            m_ballsToDelete.push_back(ball);
            it = m_activeBalls.erase(it);
        }
        else
        {
            ++it;
        }
    }
}
