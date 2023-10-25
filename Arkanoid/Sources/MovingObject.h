#pragma once
#include <Rect.h>
#include <Vec2D.h>

class MovingObject
{
public:
    bool CheckCollisionWith(const MovingObject& other);
    void Stop();

    void GetTransform(Rect<float>* transform);
    float GetX() const { return m_transform.x; }
    float GetY() const { return m_transform.y; }
    float GetW() const { return m_transform.w; }
    float GetH() const { return m_transform.h; }

    void GetPosition(float* px, float* py);
    void SetPosition(float x, float y);

    void GetVelocity(Vec2D* velocity);
    void SetVelocity(Vec2D velocity);

protected:
    Rect<float> m_transform;
    Vec2D m_velocity;
};