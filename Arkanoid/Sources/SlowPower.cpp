#include "SlowPower.h"
#include "Log.h"

SlowPower::SlowPower()
{
}

SlowPower::SlowPower(float x, float y) : Power(x, y)
{
}

void SlowPower::Init()
{
    Power::Init();
    m_animation.AddClip("fall", 36, 6, 0.1f);
    m_animation.Play("fall", true);
}

void SlowPower::Activate()
{
    if (OnActivatePower)
    {
        OnActivatePower->Invoke<PowerEvent>();
    }
}

std::string SlowPower::ToString()
{
    return "Slow Capsule (6)";
}

void SlowPower::SetDelegate(CDelegate* delegate)
{
    OnActivatePower = delegate;
}
