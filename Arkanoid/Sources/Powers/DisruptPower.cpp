#include "Powers\DisruptPower.h"

DisruptPower::DisruptPower()
{
}

DisruptPower::DisruptPower(float x, float y) : Power(x, y)
{
}

void DisruptPower::Init()
{
    Power::Init();
    m_animation.AddClip("fall", 12, 6, 0.1f);
    m_animation.Play("fall", true);
}

void DisruptPower::Activate()
{
    if (OnActivatePower)
    {
        OnActivatePower->Invoke<PowerEvent>();
    }
}

std::string DisruptPower::ToString()
{
    return "Disrupt Capsule (2)";
}

void DisruptPower::SetDelegate(CDelegate* delegate)
{
    OnActivatePower = delegate;
}
