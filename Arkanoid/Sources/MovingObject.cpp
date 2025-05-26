#include "MovingObject.h"

bool MovingObject::CheckCollisionWith(const MovingObject& other)
{
    return MathUtils::CheckRects(m_transform, other.m_transform);
}

void MovingObject::Stop()
{
    m_velocity.Set(0.0f, 0.0f);
}

void MovingObject::GetTransform(Rect<float>* transform)
{
    *transform = m_transform;
}

void MovingObject::GetPosition(float* px, float* py)
{
    *px = m_transform.x;
    *py = m_transform.y;
}

void MovingObject::SetPosition(float x, float y)
{
    m_transform.x = x;
    m_transform.y = y;
}

void MovingObject::GetVelocity(Vec2<float>* velocity)
{
    *velocity = m_velocity;
}

void MovingObject::SetVelocity(Vec2<float> velocity)
{
    m_velocity.x = velocity.x;
    m_velocity.y = velocity.y;
}
