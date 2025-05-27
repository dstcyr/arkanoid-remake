#pragma once
#include "MovingObject.h"
#include "Config.h"

class Ball : public MovingObject
{
public:
    Ball();
    Ball(float x, float y);

    void SetAngle(float angle);
    float GetAngle() const;
    void SetSpeed(float speed);
};