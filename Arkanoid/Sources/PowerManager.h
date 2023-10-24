#pragma once
#include <vector>
#include "Power.h"
#include "Delegate.h"

class PowerManager
{
public:
    void TrySpawnCapsule(int ballCount, float x, float y, bool warpDoor);
    void Clear();
    void Update(float dt);
    void Render();

    CDelegate OnActivateSlowPower;
    CDelegate OnActivateDistruptPower;
    CDelegate OnActivateBreakPower;

private:
    std::vector<Power*> m_fallingCapsules;
    Power* m_currentPower;

    bool IsCapsuleFalling() const;
    bool CanSpawnCapsule(int ballCount) const;
    void Spawn(float x, float y, bool warpDoor);
};