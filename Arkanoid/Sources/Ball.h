#pragma once
#include "Config.h"
#include "Vec2D.h"
#include "Rect.h"
#include "Delegate.h"

class Grid;
class Ball;

struct BallEvent : public Event
{
    BallEvent() : BallEvent(nullptr, 0.0f, 0.0f) {}
    BallEvent(Ball* ball, float x, float y) : ball(ball), x(x), y(y) {}
    Ball* ball;
    float x;
    float y;
};

class Ball
{
public:
    Ball();
    void Initialize();
    void Update(float dt, const Rect<float>& bounds, Grid& grid);
    void Render();
    void SetAngle(float angle);
    void GetTransform(Rect<float>* transform);
    void GetVelocity(Vec2D* velocity);
    void SetSpeed(float speed);
    void ChangeDirection(int h, int v);
    void SetPosition(float x, float y);
    bool CheckCollisionWithRect(const Rect<float>& rect);

    CDelegate OnBlockDestroyed;
    CDelegate OnBottomReached;

private:
    Rect<float> m_transform;
    Vec2D m_velocity;
    size_t m_ballTexture;
    size_t m_brickHitSound;

    void CheckBallCollisionWithBounds(const Rect<float>& bounds, float* px, float* py);
    bool CheckBallCollisionWithGrid(Grid& grid, float px, float py);
};