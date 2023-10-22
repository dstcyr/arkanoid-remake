#pragma once
#include "Config.h"
#include "Rect.h"
#include "Delegate.h"
#include <vector>
#include "TaskManager.h"

class Ball;
class CapsuleState;
class Capsule;

class LaserEvent : public Event
{
public:
    LaserEvent() : LaserEvent(0.0f, 0.0f, 0.0f, 0.0f, 0.0f) {}
    LaserEvent(float leftStartX, float leftStartY, float rightStartX, float rightStartY, float startV) :
        leftStartX(leftStartX), leftStartY(leftStartY), rightStartX(rightStartX), rightStartY(rightStartY), startV(startV) {}

    float leftStartX;
    float leftStartY;
    float rightStartX;
    float rightStartY;
    float startV;
};

class PaddleEvent : public Event
{

};

class Paddle
{
public:
    Paddle();
    void Initialize();
    void Update(float dt, float minX, float maxX);
    void Render();
    void CheckCollisionWith(Ball* ball);
    void GetTransform(Rect<float>* transform);
    void SetSticky(bool activate);

public:
    CDelegate OnLaserShotDelegate;
    CDelegate OnExitLevelDelegate;

private:
    Rect<float> m_transform;
    float m_translationElapsed;
    float m_translationDelay;
    float m_velocity;
    size_t m_vausLeftPart;
    size_t m_vausRightPart;
    size_t m_vausMiddlePart;
    size_t m_laserMiddleLeftPart;
    size_t m_laserMiddleRightPart;
    size_t m_laserLeftPart;
    size_t m_laserRightPart;
    size_t m_hitSound;
    float m_expandElapsed;
    float m_expandSpeed;
    float m_armingElapsed;
    float m_expandTargetWidth;
    float m_contractTargetWidth;
    float m_sideWidth;
    bool m_armed;
    bool m_sticky;
    std::vector<Ball*> m_balls;

    void UpdateControls(float dt, float* px);

public:
    bool TaskExpandShip(float dt, CTaskState* state);
    bool TaskContractShip(float dt, CTaskState* state);
    bool TaskActivateLaser(float dt, CapsuleState* state);
    bool TaskDeactivateLaser(float dt, CapsuleState* state);

private:
    bool UpdateLaserTask(float dt, bool activate, Capsule* capsule);
};