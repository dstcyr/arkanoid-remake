#pragma once
#include "Power.h"

// Laser Power
// 4: red - increase to Vaus laser fire power
//        {   L   }
// Collect the red capsule to transform the Vaus into its Laser-firing configuration. In
// this form, you can fire lasers at the top of the screen by pushing the fire button.
// Lasers can be used against every brick except Gold bricks, and against debris. Silver
// bricks can only be destroyed by lasers when they are hit the required number of times.
class LaserPower : public Power
{
public:
    LaserPower();
    LaserPower(float x, float y);
    static int GetID() { return 4; }

    void Init() override;
    void Activate() override;
    void Deactivate() override;
    std::string ToString() override;
    int GetInstanceID() const override { return LaserPower::GetID(); }
};
