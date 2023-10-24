#include "ExpandPower.h"
#include "World.h"
#include "Ship.h"
#include "Log.h"

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
    Ship* ship = World::Get().GetShip();
    CHECK(ship);
    ship->ExpandShip();
}

void ExpandPower::Deactivate()
{
    Ship* ship = World::Get().GetShip();
    CHECK(ship);
    ship->ContractShip();
}

std::string ExpandPower::ToString()
{
    return "Expand Capsule (3)";
}
