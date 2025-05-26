#pragma once
#include "maths/Rect.h"
#include "maths/Vec2.h"

class MovingObject
{
public:
    bool CheckCollisionWith(const MovingObject& other);
    void Stop();

    void GetTransform(Rect<float>* transform);
    float GetX() const { return m_transform.x; }
    float GetY() const { return m_transform.y; }
    float GetW() const { return m_transform.width; }
    float GetH() const { return m_transform.height; }

    void GetPosition(float* px, float* py);
    void SetPosition(float x, float y);

    void GetVelocity(Vec2<float>* velocity);
    void SetVelocity(Vec2<float> velocity);

protected:
    Rect<float> m_transform;
    Vec2<float> m_velocity;
};