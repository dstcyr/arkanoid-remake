#pragma once
#include "Power.h"
#include "Delegate.h"

// Catch Power
// 1: yellow / green - catch the ball and release it
//        {   C   }
// Collect the green capsule to gain the catch ability. When the ball hits the Vaus, it
// will stick to the surface. Press the Fire button to release the ball. The ball will
// automatically release after a certain period of time has passed.
class CatchPower : public Power
{
public:
public:
    CatchPower();
    CatchPower(float x, float y);
    static int GetID() { return 1; }

    void Init() override;
    void Activate() override;
    void Deactivate() override;
    std::string ToString() override;
    int GetInstanceID() const override { return CatchPower::GetID(); }
    void SetDelegate(CDelegate* delegate);

private:
    CDelegate* OnActivatePower;
};