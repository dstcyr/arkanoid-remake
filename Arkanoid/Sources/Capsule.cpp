#include "Capsule.h"
#include "Log.h"
#include "Engine.h"
#include "Config.h"
#include <cstdlib>
#include "Game.h"

// Capsule can spawn only one at a time
// No capsule spawns when there are multiple balls

Capsule::Capsule() : Capsule(0.0f, 0.0f)
{
    m_transform = Rect<float>();
    m_velocity = 0.0f;
    m_activated = false;
}

Capsule::Capsule(float x, float y)
{
    m_transform.x = x;
    m_transform.y = y;
    m_transform.w = BLOCK_WIDTH;
    m_transform.h = BLOCK_HEIGHT;
    m_velocity = CAPSULE_FALLING_SPEED;
    m_activated = false;
}

void Capsule::Update(float dt)
{
    m_transform.y += m_velocity * dt;
    m_animation.Update(dt);
}

void Capsule::Render()
{
    m_animation.Render(m_transform);
}

void Capsule::GetTransform(Rect<float>* transform)
{
    *transform = m_transform;
}

Capsule* Capsule::Spawn(Capsule* activePower, float x, float y)
{
    int activePowerId = -1;
    if (activePower)
    {
        activePowerId = activePower->GetId();
    }

    // When the game decides to spawn a powerup, it first generates a one - byte random
    // number(RN).It checks that RN against a special table of six arbitrary values.
    unsigned char randomNumber = rand() % 256;

#if ALWAYS_SPAWN_WARP
    Capsule* breakCapsule = new BreakCapsule(x, y);
    breakCapsule->Init();
    breakCapsule->m_animation.Play("fall", true);
    return breakCapsule;
#endif

    // PlayerCapsule is not activated but I don't want to spawn it 2 times in a row
    if (randomNumber >= 0 && randomNumber <= 2 && activePowerId != PlayerCapsule::GetCapsuleId())
    {
        // If it matches one of the first three, the powerup is an extra life (PlayerCapsule);
        return new PlayerCapsule(x, y);
    }

    // BreakCapsule is not activated but I don't want to spawn it 2 times in a row
    if (randomNumber >= 4 && randomNumber <= 6 && activePowerId != BreakCapsule::GetCapsuleId())
    {
        // If it matches one of the last three, it's a warp powerup.
        Capsule* breakCapsule = new BreakCapsule(x, y);
        breakCapsule->Init();
        breakCapsule->m_animation.Play("fall", true);
        return breakCapsule;
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
    static int mapSelectedToCapsule[] = {
        ExpandCapsule::GetCapsuleId(),
        SlowCapsule::GetCapsuleId(),
        CatchCapsule::GetCapsuleId(),
        ExpandCapsule::GetCapsuleId(),
        DisruptCapsule::GetCapsuleId(),
        LaserCapsule::GetCapsuleId(),
        DisruptCapsule::GetCapsuleId(),
        -1
    };

    // 0x07 represents the binary pattern 00000111
    unsigned char selectedCapsule = randomNumber & 0x07;

    // Finally, if the game finishes all this and you already have the powerup it rolled, it starts 
    // the process all over again until it gets one you don't have.
    while (mapSelectedToCapsule[selectedCapsule] == activePowerId)
    {
        randomNumber = rand() % 256;
        selectedCapsule = randomNumber & 0x07;
    }

#if ALWAYS_SPAWN_EXPAND
    selectedCapsule = 0;
#elif ALWAYS_SPAWN_SLOW
    selectedCapsule = 1;
#elif ALWAYS_SPAWN_CATCH
    selectedCapsule = 2;
#elif ALWAYS_SPAWN_DISRUPT
    selectedCapsule = 4;
#elif ALWAYS_SPAWN_LASER
    selectedCapsule = 5;
#endif

    Capsule* newCapsule = nullptr;
    if (mapSelectedToCapsule[selectedCapsule] != activePowerId)
    {
        switch (selectedCapsule)
        {
        case 0:
            newCapsule = new ExpandCapsule(x, y);
            break;

        case 1:
            newCapsule = new SlowCapsule(x, y);
            break;

        case 2:
            newCapsule = new CatchCapsule(x, y);
            break;

        case 3:
            newCapsule = new ExpandCapsule(x, y);
            break;

        case 4:
            newCapsule = new DisruptCapsule(x, y);
            break;

        case 5:
            newCapsule = new LaserCapsule(x, y);
            break;

        case 6:
            newCapsule = new DisruptCapsule(x, y);
            break;
        }

        if (newCapsule)
        {
            newCapsule->Init();
            newCapsule->m_animation.Play("fall", true);
        }
    }

    // If the last bits are 00000111, the number is 7 and there is no case for that.
    return newCapsule;
}

bool Capsule::CanSpawn(int ballCount)
{
    // No colored capsules will fall as long as there is more than one ball in play.
    if (ballCount > 1)
    {
        return false;
    }

    // when a block is destroyed, a power-up will spawn 20% of the time
    // Generate a random number between 0 and 100
    int randomNumber = rand() % 101;
    return randomNumber >= 0 && randomNumber <= CHANCE_TO_SPAWN_CAPSULE;
}

void Capsule::Init()
{
    m_animation.Init("Assets/Images/capsules.png", 6, BLOCK_WIDTH, BLOCK_HEIGHT);
}

void Capsule::Deactivate(Game* game)
{
}

void Capsule::OnActivated()
{
    m_activated = true;
}

void Capsule::OnDeactivated()
{
    m_activated = false;
}

bool Capsule::IsActive()
{
    return m_activated;
}

BreakCapsule::BreakCapsule() : Capsule()
{
}

BreakCapsule::BreakCapsule(float x, float y) : Capsule(x, y)
{
}

// 0 : pink - opens warp escape - advance to next round
void BreakCapsule::Render()
{
    Capsule::Render();

#if SHOW_DEBUG_CAPSULE
    Engine::FillRect(m_transform.x, m_transform.y, m_transform.w, m_transform.h, NColor::Pink);
#endif
}

std::string BreakCapsule::ToString()
{
    return "Break Capsule (0)";
}

void BreakCapsule::Activate(Game* game)
{
    CHECK(game);
    game->OpenTheWarpDoors();
}

void BreakCapsule::Deactivate(Game* game)
{
    CHECK(game);
    game->CloseTheWarpDoors();
}

void BreakCapsule::Init()
{
    Capsule::Init();
    m_animation.AddClip("fall", 0, 6, 0.1f);
}

CatchCapsule::CatchCapsule() : Capsule()
{
}

CatchCapsule::CatchCapsule(float x, float y) : Capsule(x, y)
{
}

// 1: yellow / green - catch the ball and release it
void CatchCapsule::Render()
{
    Capsule::Render();

#if SHOW_DEBUG_CAPSULE
    Engine::FillRect(m_transform.x, m_transform.y, m_transform.w, m_transform.h, NColor::LightYellowGreen);
#endif
}

std::string CatchCapsule::ToString()
{
    return "Catch Capsule (1)";
}

void CatchCapsule::Activate(Game* game)
{
    CHECK(game);
    game->ActivateCatch(true);
}

void CatchCapsule::Deactivate(Game* game)
{
    CHECK(game);
    game->ActivateCatch(false);
}

void CatchCapsule::Init()
{
    Capsule::Init();
    m_animation.AddClip("fall", 6, 6, 0.1f);
}

DisruptCapsule::DisruptCapsule() : Capsule()
{
}

DisruptCapsule::DisruptCapsule(float x, float y) : Capsule(x, y)
{
}

// 2: light blue - splits energy ball into 3 particles
void DisruptCapsule::Render()
{
    Capsule::Render();

#if SHOW_DEBUG_CAPSULE
    Engine::FillRect(m_transform.x, m_transform.y, m_transform.w, m_transform.h, NColor::LightBlue);
#endif
}

std::string DisruptCapsule::ToString()
{
    return "Disrupt Capsule (2)";
}

void DisruptCapsule::Activate(Game* game)
{
    CHECK(game);

    float x, y;
    if (game->GetCurrentBallPosition(&x, &y))
    {
        float speed = game->GetGameSpeed();
        Ball* ball1 = game->AddBall(x, y);
        ball1->SetSpeed(speed);
        ball1->SetAngle(SHARP_ANGLE);
        ball1->ChangeDirection(1, -1);

        Ball* ball2 = game->AddBall(x, y);
        ball2->SetSpeed(speed);
        ball2->SetAngle(SHARP_ANGLE);
        ball2->ChangeDirection(-1, -1);
    }
}

void DisruptCapsule::Init()
{
    Capsule::Init();
    m_animation.AddClip("fall", 12, 6, 0.1f);
}

ExpandCapsule::ExpandCapsule() : Capsule()
{
}

ExpandCapsule::ExpandCapsule(float x, float y) : Capsule(x, y)
{
}

// 3: dark blue / purple - widens the Vaus controller
void ExpandCapsule::Render()
{
    Capsule::Render();

#if SHOW_DEBUG_CAPSULE
    Engine::FillRect(m_transform.x, m_transform.y, m_transform.w, m_transform.h, NColor::DarkBlue);
#endif
}

std::string ExpandCapsule::ToString()
{
    return "Expand Capsule (3)";
}

void ExpandCapsule::Activate(Game* game)
{
    CHECK(game);
    game->ExpandVaus();
}

void ExpandCapsule::Deactivate(Game* game)
{
    CHECK(game);
    game->StandardVaus();
}

void ExpandCapsule::Init()
{
    Capsule::Init();
    m_animation.AddClip("fall", 18, 6, 0.1f);
}

LaserCapsule::LaserCapsule() : Capsule()
{
}

LaserCapsule::LaserCapsule(float x, float y) : Capsule(x, y)
{
}

// 4: red - increase to Vaus laser fire power
void LaserCapsule::Render()
{
    Capsule::Render();

#if SHOW_DEBUG_CAPSULE
    Engine::FillRect(m_transform.x, m_transform.y, m_transform.w, m_transform.h, NColor::DarkRed);
#endif
}

std::string LaserCapsule::ToString()
{
    return "Laser Capsule (4)";
}

void LaserCapsule::Activate(Game* game)
{
    CHECK(game);
    game->ActivateLaser(this);
}

void LaserCapsule::Deactivate(Game* game)
{
    CHECK(game);
    game->DeactivateLaser(this);
}

void LaserCapsule::Init()
{
    Capsule::Init();
    m_animation.AddClip("fall", 24, 6, 0.1f);
}

PlayerCapsule::PlayerCapsule() : Capsule()
{
}

PlayerCapsule::PlayerCapsule(float x, float y) : Capsule(x, y)
{
}

// 5: grey - awards an extra Vaus
void PlayerCapsule::Render()
{
    Capsule::Render();

#if SHOW_DEBUG_CAPSULE
    Engine::FillRect(m_transform.x, m_transform.y, m_transform.w, m_transform.h, NColor::MediumGray);
#endif
}

std::string PlayerCapsule::ToString()
{
    return "Player Capsule (5)";
}

void PlayerCapsule::Activate(Game* game)
{
    CHECK(game);
    game->AddVaus();
}

void PlayerCapsule::Init()
{
    Capsule::Init();
    m_animation.AddClip("fall", 30, 6, 0.1f);
}

SlowCapsule::SlowCapsule() : Capsule()
{
}

SlowCapsule::SlowCapsule(float x, float y) : Capsule(x, y)
{
}

// 6: orange - slows movement of ball
void SlowCapsule::Render()
{
    Capsule::Render();

#if SHOW_DEBUG_CAPSULE
    Engine::FillRect(m_transform.x, m_transform.y, m_transform.w, m_transform.h, NColor::Orange);
#endif
}

std::string SlowCapsule::ToString()
{
    return "Slow Capsule (6)";
}

void SlowCapsule::Activate(Game* game)
{
    float gameSpeed = game->GetGameSpeed();
    game->SetGameSpeed(gameSpeed / 2.0f);
}

void SlowCapsule::Deactivate(Game* game)
{
    float gameSpeed = game->GetGameSpeed();
    game->SetGameSpeed(gameSpeed * 2.0f);
}

void SlowCapsule::Init()
{
    Capsule::Init();
    m_animation.AddClip("fall", 36, 6, 0.1f);
}
