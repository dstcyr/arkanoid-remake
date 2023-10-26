#pragma once
#include <vector>
#include "Ball.h"
#include "Delegate.h"
#include "Ship.h"

struct BallEvent : public Event
{
    BallEvent() : BallEvent(0.0f, 0.0f) {}
    BallEvent(float x, float y) : x(x), y(y) {}
    float x = 0.0f;
    float y = 0.0f;
};

enum ECollisionBoundType
{
    WALL_COLLISION,
    TOP_COLLISION,
    BOTTOM_COLLISION,
    NO_COLLISION
};

struct CatchInfo
{
    Ball* ball;
    float offsetX;
    float offsetY;
};

class BallManager
{
public:
    void Initialize();
    void SpawnBall(int count = 1);
    void Update(float dt);
    void Render();
    int Count();
    void SlowDown();
    void Clear();
    void Remove(Ball* ball);
    void CatchingBall(bool catching);
    void FirstBallOfTheRound();
    bool CheckCollisionWith(MovingObject& other);

    CDelegate OnBottomReached;

private:
    float m_gameSpeed = GAME_SPEED;
    std::vector<Ball*> m_activeBalls;
    std::vector<Ball*> m_ballsToDelete;
    std::vector<CatchInfo> m_caughtBalls;
    float m_slowdownElapsed;
    size_t m_shipCollisionSfx = 0;
    size_t m_blockCollisionSfx = 0;
    size_t m_hardBlockCollisionSfx = 0;
    size_t m_ballTexture = 0;
    bool m_catchingBalls;
    bool m_firstBall;
    float m_firstBallElapsed;

    void SetGameSpeed(float speed);
    ECollisionBoundType CheckCollisionWithBounds(float* px, float* py, Rect<float>& transform, Vec2D& velocity);
    ECollisionResult CheckCollisionWithShip(float* px, float* py, Ship* ship, Rect<float>& transform, Vec2D& velocity);
    float CheckCollisionWithGrid(float* px, float* py, Rect<float>& transform, Vec2D& velocity);
    void PlayHitBlockSfx(int hit);
    void PlayHitShipSFX();
    void ProcessInput();
    void ForceLaunchCaughtBalls();
};