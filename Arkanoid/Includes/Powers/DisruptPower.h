#pragma once
#include "Power.h"
#include "utils/Delegate.h"

// Disrupt Power
// 2: light blue - splits energy ball into 3 particles
//        {   D   }
// Collect the cyan capsule to cause the ball to split into three instances of itself.
// All three balls can be kept aloft. There is no penalty for losing the first two balls.
// No colored capsules will fall as long as there is more than one ball in play. This is 
// the only power up that, while in effect, prevents other power ups from falling.
class DisruptPower : public Power
{
public:
    DisruptPower();
    DisruptPower(float x, float y);
    static int GetID() { return 2; }

    void Init() override;
    void Activate() override;
    std::string ToString() override;
    int GetInstanceID() const override { return DisruptPower::GetID(); }

    void SetDelegate(CDelegate* delegate);

private:
    CDelegate* OnActivatePower;
};
