#include "Power.h"
#include "Config.h"

Power::Power() : Power(0.0f, 0.0f)
{
}

Power::Power(float x, float y)
{
    m_transform.Set(x, y, BLOCK_WIDTH, BLOCK_HEIGHT);
    m_velocity.Set(0.0f, CAPSULE_FALLING_SPEED);
}

void Power::Update(float dt)
{
    m_transform.y += m_velocity.y * dt;
    m_animation.Update(dt);
}

void Power::Render()
{
    m_animation.Draw(m_transform);
}

void Power::Init()
{
    m_animation.Load("Images/capsules.png");
    m_animation.Init(6, BLOCK_WIDTH, BLOCK_HEIGHT);
}

void Power::Activate()
{
}

void Power::Deactivate()
{
}
