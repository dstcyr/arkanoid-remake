#pragma once
#include "MovingObject.h"
#include "Config.h"
#include "Delegate.h"

class Ball;

struct BallEvent : public Event
{
    BallEvent() : BallEvent(nullptr, 0.0f, 0.0f) {}
    BallEvent(Ball* ball, float x, float y) : ball(ball), x(x), y(y) {}
    Ball* ball = nullptr;
    float x = 0.0f;
    float y = 0.0f;
};

class Ball : public MovingObject
{
public:
    Ball();
    Ball(float x, float y);
    void Initialize();
    void Update(float dt);
    void Render();
    void SetAngle(float angle);
    float GetAngle() const;
    void SetSpeed(float speed);

    CDelegate OnBottomReached;
    CDelegate OnBounced;

private:
    size_t m_ballTexture = 0;

    bool CheckCollisionWithBounds(float* px, float* py);
    bool CheckCollisionWithShip(float* px, float* py);
    bool CheckCollisionWithGrid(float* px, float* py);
};