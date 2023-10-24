#include "MovingObject.h"
#include "Collision.h"

bool MovingObject::CheckCollisionWith(const MovingObject& other)
{
    return Engine::CheckRects(m_transform, other.m_transform);
}

void MovingObject::Stop()
{
    m_velocity.Set(0.0f, 0.0f);
}

void MovingObject::GetTransform(Rect<float>* transform)
{
    *transform = m_transform;
}
