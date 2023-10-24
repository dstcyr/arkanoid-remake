#pragma once
#include "Power.h"
#include "Delegate.h"

// Expand Power
// 3: dark blue / purple - widens the Vaus controller
//        {   E   }
// Collect the blue capsule to extend the width of the Vaus.
class ExpandPower : public Power
{
public:
    ExpandPower();
    ExpandPower(float x, float y);
    static int GetID() { return 3; }

    void Init() override;
    void Activate() override;
    void Deactivate() override;
    std::string ToString() override;
    int GetInstanceID() const override { return ExpandPower::GetID(); }
};
