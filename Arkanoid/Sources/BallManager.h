#pragma once
#include <vector>
#include "Ball.h"
#include "Delegate.h"

class BallManager
{
public:
    void SpawnBall(int count = 1);
    void Update(float dt);
    void Render();
    void Destroy();
    int Count();
    void SlowDown();
    void Clear();
    void Remove(Ball* ball);

    CDelegate OnBallReachedBottom;

private:
    float m_gameSpeed = GAME_SPEED;
    std::vector<Ball*> m_activeBalls;
    std::vector<Ball*> m_ballsToDelete;
    float m_slowdownElapsed;

    void OnBallReachedBottomCallback(const BallEvent& ballEvent);
    void SetGameSpeed(float speed);
};