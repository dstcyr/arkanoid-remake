#include "CatchPower.h"

CatchPower::CatchPower()
{
}

CatchPower::CatchPower(float x, float y) : Power(x, y)
{
}

void CatchPower::Init()
{
    Power::Init();
    m_animation.AddClip("fall", 6, 6, 0.1f);
    m_animation.Play("fall", true);
}

void CatchPower::Activate()
{
    if (OnActivatePower)
    {
        OnActivatePower->Invoke<PowerEvent>(true);
    }
}

void CatchPower::Deactivate()
{
    if (OnActivatePower)
    {
        OnActivatePower->Invoke<PowerEvent>(false);
    }
}

std::string CatchPower::ToString()
{
    return "Catch Capsule (1)";
}

void CatchPower::SetDelegate(CDelegate* delegate)
{
    OnActivatePower = delegate;
}
