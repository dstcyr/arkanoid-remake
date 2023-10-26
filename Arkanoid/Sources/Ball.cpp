#include "Ball.h"
#include "Engine.h"
#include "Log.h"
#include "Ship.h"
#include "World.h"
#include "Collision.h"
#include "Stopwatch.h"
#include "Config.h"
#include "MathUtils.h"

Ball::Ball() : Ball(BALL_START_X, BALL_START_Y)
{
}

Ball::Ball(float x, float y)
{
    m_transform.x = x;
    m_transform.y = y;
    m_transform.w = 16.0f;
    m_transform.h = 16.0f;
}

void Ball::SetAngle(float angle)
{
    if (Engine::NearEqual(std::abs(angle), 0.0f) || Engine::NearEqual(std::abs(angle), 180.0f))
    {
        angle = -65.0f;
    }

    float length = m_velocity.Length();
    if (length == 0.0f)
    {
        m_velocity.Set(0.0f, 1.0f);
        length = 1.0f;
    }

    m_velocity.Normalize();
    m_velocity.SetRotation(angle);
    m_velocity *= length;
}

void Ball::SetSpeed(float speed)
{
    m_velocity.Normalize();
    m_velocity *= speed;
}

float Ball::GetAngle() const
{
    return m_velocity.GetAngle();
}
