#pragma once
#include "Power.h"
#include "utils/Delegate.h"

// Break Power
// 0 : pink - opens warp escape - advance to next round
//        {   B   }
// Collect the violet capsule to create a "break out" exit on the right side of the
// stage.Passing through this exit will cause you to advance to the next stage 
// immediately, as well as earn a 10, 000 point bonus.
class BreakPower : public Power
{
public:
public:
    BreakPower();
    BreakPower(float x, float y);
    static int GetID() { return 0; }

    void Init() override;
    void Activate() override;
    std::string ToString() override;
    int GetInstanceID() const override { return BreakPower::GetID(); }
    void SetDelegate(CDelegate* delegate);

private:
    CDelegate* OnActivatePower;
};