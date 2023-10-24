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

    CDelegate OnBallReachedBottom;

private:
    float m_gameSpeed = GAME_SPEED;
    std::vector<Ball*> m_activeBalls;
    float m_slowdownElapsed;

    void OnBallReachedBottomCallback(const BallEvent& ballEvent);
    void SetGameSpeed(float speed);
};