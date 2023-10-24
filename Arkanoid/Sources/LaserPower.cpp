#include "LaserPower.h"
#include "World.h"
#include "Ship.h"
#include "Log.h"

LaserPower::LaserPower()
{
}

LaserPower::LaserPower(float x, float y) : Power(x, y)
{
}

void LaserPower::Init()
{
    Power::Init();
    m_animation.AddClip("fall", 24, 6, 0.1f);
    m_animation.Play("fall", true);
}

void LaserPower::Activate()
{
    Ship* ship = World::Get().GetShip();
    CHECK(ship);
    ship->ActivateLaser();
}

void LaserPower::Deactivate()
{
    Ship* ship = World::Get().GetShip();
    CHECK(ship);
    ship->DeactivateLaser();
}

std::string LaserPower::ToString()
{
    return "Laser Capsule (4)";
}
