#pragma once
#include "Power.h"

// Player Power
// 5: grey - awards an extra Vaus
//        {    P   }
// Collect the gray capsule to earn an extra Vaus.
class PlayerPower : public Power
{
public:
    PlayerPower();
    PlayerPower(float x, float y);

    static int GetID() { return 5; }

    void Init() override;
    void Activate() override;
    std::string ToString() override;
    int GetInstanceID() const override { return PlayerPower::GetID(); }

private:
    size_t m_LifeSfx;
};