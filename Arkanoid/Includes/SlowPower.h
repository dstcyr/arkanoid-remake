#pragma once
#include "Power.h"
#include "utils/Delegate.h"

// Slow Power
// 6: orange - slows movement of ball
//        {   S   }
// Collect the orange capsule to slow the velocity at which the ball moves.
// The slow effect is usually temporary and lasts for a certain period of 
// time. Once the time duration is over, the ball returns to its normal speed.
class SlowPower : public Power
{
public:
    SlowPower();
    SlowPower(float x, float y);
    static int GetID() { return 6; }

    void Init() override;
    void Activate() override;
    std::string ToString() override;
    void SetDelegate(CDelegate* delegate);
    int GetInstanceID() const override { return SlowPower::GetID(); }

private:
    CDelegate* OnActivatePower;
};
