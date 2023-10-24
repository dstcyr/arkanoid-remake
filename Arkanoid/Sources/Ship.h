#pragma once
#include "MovingObject.h"
#include "Config.h"
#include "TaskManager.h"
#include "Delegate.h"

class Ball;

enum ECollisionResult
{
    LEFT_SIDE_HIT,
    RIGHT_SIDE_HIT,
    LEFT_ZONE_HIT,
    RIGHT_ZONE_HIT,
    MIDDLE_HIT,
    LEFT_LOW_HIT,
    RIGHT_LOW_HIT,
    NO_HIT
};

class LaserEvent : public Event
{
public:
    LaserEvent() : LaserEvent(0.0f, 0.0f, 0.0f, 0.0f) {}
    LaserEvent(float leftStartX, float leftStartY, float rightStartX, float rightStartY) :
        leftStartX(leftStartX), leftStartY(leftStartY), rightStartX(rightStartX), rightStartY(rightStartY) {}

    float leftStartX;
    float leftStartY;
    float rightStartX;
    float rightStartY;
};

class ShipState : public CTaskState
{
public:
    ShipState()
    {
        elapsed = 0.0f;
        phase = 0;
        firstPass = true;
    }

    float elapsed;
    int phase;
    bool firstPass;
};

class Ship : public MovingObject
{
public:
    Ship();
    Ship(float x, float y);
    void Initialize();
    void Update(float dt);
    void Render();

    ECollisionResult GetCollisionResponse(float x, float y);

    void ExpandShip();
    void ContractShip();
    void ActivateLaser();
    void DeactivateLaser();

    CDelegate OnLaserShotDelegate;

private:
    size_t m_vausLeftPart;
    size_t m_vausRightPart;
    size_t m_vausMiddlePart;
    size_t m_laserMiddleLeftPart;
    size_t m_laserMiddleRightPart;
    size_t m_laserLeftPart;
    size_t m_laserRightPart;
    size_t m_expandSfx;
    float m_sideWidth;
    float m_middleWidth;
    float m_expandSpeed;
    float m_contractTargetWidth;
    float m_expandTargetWidth;
    bool m_LaserActivated;

    CTaskManager m_sequence;

    void UpdateControls(float dt, float* px);
    void CheckCollisionWithBounds(float* px);
    void GetZoneDelimiters(float* startA, float* endA, float* startB, float* endB);
    
    bool TaskExpandShip(float dt, ShipState* state);
    bool TaskContractShip(float dt, ShipState* state);
    bool TaskActivateLaser(float dt, ShipState* state);
    bool TaskDeactivateLaser(float dt, ShipState* state);
    void ProcessShipInput();
    void RenderShipPart(size_t id, float x, float y, float w, float h);
};