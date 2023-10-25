#include "BreakPower.h"

BreakPower::BreakPower()
{
}

BreakPower::BreakPower(float x, float y) : Power(x, y)
{
}

void BreakPower::Init()
{
    Power::Init();
    m_animation.AddClip("fall", 0, 6, 0.1f);
    m_animation.Play("fall", true);
}

void BreakPower::Activate()
{
    if (OnActivatePower)
    {
        OnActivatePower->Invoke<PowerEvent>();
    }
}

std::string BreakPower::ToString()
{
    return "Break Capsule (0)";
}

void BreakPower::SetDelegate(CDelegate* delegate)
{
    OnActivatePower = delegate;
}
