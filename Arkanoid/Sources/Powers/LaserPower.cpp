#include "Powers\LaserPower.h"
#include "GameLevel.h"
#include "Ship.h"
#include "utils/Checks.h"

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
    Ship* ship = GameLevel::Get().GetShip();
    BX_CHECKS(ship, "Invalid ship");
    ship->ActivateLaser();
}

void LaserPower::Deactivate()
{
    Ship* ship = GameLevel::Get().GetShip();
    BX_CHECKS(ship, "Invalid ship");
    ship->DeactivateLaser();
}

std::string LaserPower::ToString()
{
    return "Laser Capsule (4)";
}
