#include "PowerManager.h"
#include "Log.h"
#include "Config.h"
#include "PlayerPower.h"
#include "World.h"
#include "Ship.h"
#include "SlowPower.h"
#include "DisruptPower.h"
#include "ExpandPower.h"
#include "LaserPower.h"
#include "BreakPower.h"
#include "CatchPower.h"

bool PowerManager::IsCapsuleFalling() const
{
    return false;
}

bool PowerManager::CanSpawnCapsule(int ballCount) const
{
    int fallingCapsuleCount = static_cast<int>(m_fallingCapsules.size());
    if (fallingCapsuleCount > 0)
    {
        return false;
    }

    // No colored capsules will fall as long as there is more than one ball in play.
    if (ballCount > 1)
    {
        return false;
    }

    // When a block is destroyed, a power-up will spawn 20% of the time
    int randomNumber = rand() % 101;
    return randomNumber >= 0 && randomNumber <= CHANCE_TO_SPAWN_CAPSULE;
}

void PowerManager::Spawn(float x, float y, bool warpDoor)
{
    int activePowerId = -1;
    if (m_currentPower)
    {
        activePowerId = m_currentPower->GetInstanceID();
    }

    // When the game decides to spawn a powerup, it first generates a one - byte random
    // number(RN).It checks that RN against a special table of six arbitrary values.
    unsigned char randomNumber = rand() % 256;

    // PlayerCapsule is not activated but I don't want to spawn it 2 times in a row
    if (randomNumber >= 0 && randomNumber <= 2)
    {
        // If it matches one of the first three, the powerup is an extra life (PlayerCapsule);
        PlayerPower* pp = new PlayerPower(x, y);
        pp->Init();
        m_fallingCapsules.push_back(pp);
        return;
    }

    // BreakCapsule is not activated but I don't want to spawn it 2 times in a row
    if (randomNumber >= 4 && randomNumber <= 6 && !warpDoor)
    {
        // If it matches one of the last three, it's a warp powerup.
        BreakPower* bp = new BreakPower(x, y);
        bp->Init();
        bp->SetDelegate(&OnActivateBreakPower);
        m_fallingCapsules.push_back(bp);
        return;
    }

    // If the game didn't spawn one of the rare powerups, it moves on to the common powerup logic.
    // The game then looks at the lowest three bits of the RN and maps it to the common powerups like this:
    // 
    //      0 = ExpandCapsule
    //      1 = SlowCapsule
    //      2 = CatchCapsule
    //      3 = ExpandCapsule
    //      4 = DisruptCapsule
    //      5 = LaserCapsule
    //      6 = DisruptCapsule
    //static int mapSelectedToCapsule[] = {
    //    ExpandPower::GetID(),
    //    SlowPower::GetID(),
    //    CatchPower::GetID(),
    //    ExpandPower::GetID(),
    //    DisruptPower::GetID(),
    //    LaserPower::GetID(),
    //    DisruptPower::GetID(),
    //    -1
    //};
    static int mapSelectedToCapsule[] = {
        CatchPower::GetID(),
        CatchPower::GetID(),
        CatchPower::GetID(),
        CatchPower::GetID(),
        ExpandPower::GetID(),
        ExpandPower::GetID(),
        ExpandPower::GetID(),
        ExpandPower::GetID()
    };

    // 0x07 represents the binary pattern 00000111
    unsigned char selectedCapsule = randomNumber & 0x07;
    int capsuleToSpawn = mapSelectedToCapsule[selectedCapsule];

    // Finally, if the game finishes all this and you already have the powerup it rolled, it starts 
    // the process all over again until it gets one you don't have.
    while (capsuleToSpawn == activePowerId)
    {
        // If the last bits are 00000111, the number is 7 and there is no case for that.
        randomNumber = rand() % 256;
        selectedCapsule = randomNumber & 0x07;
        capsuleToSpawn = mapSelectedToCapsule[selectedCapsule];
    }

    Power* newPower = nullptr;
    if (capsuleToSpawn != activePowerId)
    {
        // 0 (break) and 5 (life) are already managed
        switch (capsuleToSpawn)
        {
        // ID 0 : Break
        case 0:
        {
            LOG(LL_ERROR, "The code should have exited early if spawning a Break Power");
            FAIL();
        }

        // ID 1 : Catch 
        case 1:
        {
            CatchPower* cp = new CatchPower(x, y);
            cp->SetDelegate(&OnActivateCatchPower);
            newPower = cp;
            break;
        }

        // ID 2 : Disrupt 
        case 2:
        {
            DisruptPower* dp = new DisruptPower(x, y);
            dp->SetDelegate(&OnActivateDistruptPower);
            newPower = dp;
            break;
        }

        // ID 3 : Expand 
        case 3:
        {
            newPower = new ExpandPower(x, y);
            break;
        }

        // ID 4 : Laser 
        case 4:
        {
            newPower = new LaserPower(x, y);
            break;
        }

        // ID 5 : Player 
        case 5:
        {
            LOG(LL_ERROR, "The code should have exited early if spawning a Player Power");
            FAIL();
        }

        // ID 6 : Laser 
        case 6:
        {
            SlowPower* sp = new SlowPower(x, y);
            sp->SetDelegate(&OnActivateSlowPower);
            newPower = sp;
            break;
        }
        }

        if (newPower)
        {
            newPower->Init();
            m_fallingCapsules.push_back(newPower);
        }
    }
}

void PowerManager::TrySpawnCapsule(int ballCount, float x, float y, bool warpDoor)
{
    if (CanSpawnCapsule(ballCount))
    {
        Spawn(x, y, warpDoor);
    }
}

void PowerManager::Clear()
{
    for (Power* power : m_fallingCapsules)
    {
        SAFE_DELETE(power);
    }

    SAFE_DELETE(m_currentPower);
    m_fallingCapsules.clear();
}

void PowerManager::Update(float dt)
{
    Ship* ship = World::Get().GetShip();
    CHECK(ship);

    auto it = m_fallingCapsules.begin();
    while (it != m_fallingCapsules.end())
    {
        (*it)->Update(dt);
        if ((*it)->CheckCollisionWith(*ship))
        {
            if (m_currentPower)
            {
                m_currentPower->Deactivate();
            }

            (*it)->Activate();
            m_currentPower = (*it);
            it = m_fallingCapsules.erase(it);
        }
        else
        {
            float y = (*it)->GetY();
            if (y >= BOTTOM_WALL_Y)
            {
                it = m_fallingCapsules.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }
}

void PowerManager::Render()
{
    for (Power* power : m_fallingCapsules)
    {
        power->Render();
    }
}
