#include "Powers\ExpandPower.h"
#include "GameLevel.h"
#include "Ship.h"
#include "utils/Checks.h"

ExpandPower::ExpandPower()
{
}

ExpandPower::ExpandPower(float x, float y) : Power(x, y)
{
}

void ExpandPower::Init()
{
    Power::Init();
    m_animation.AddClip("fall", 18, 6, 0.1f);
    m_animation.Play("fall", true);
}

void ExpandPower::Activate()
{
    Ship* ship = GameLevel::Get().GetShip();
    BX_CHECKS(ship, "Invalid ship");
    ship->ExpandShip();
}

void ExpandPower::Deactivate()
{
    Ship* ship = GameLevel::Get().GetShip();
    BX_CHECKS(ship, "Invalid ship");
    ship->ContractShip();
}

std::string ExpandPower::ToString()
{
    return "Expand Capsule (3)";
}
