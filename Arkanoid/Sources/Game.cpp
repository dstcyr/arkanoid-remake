#include "Game.h"
#include "Log.h"
#include "Engine.h"
#include "Capsule.h"
#include "Collision.h"
#include "Laser.h"
#include <functional>
#include "SaveGame.h"
#include "LevelData.h"
#include "Debris.h"
#include "MathUtils.h"

Game::Game()
{
    m_whiteFont = 0;
    m_orangeFont = 0;
    m_background = 0;
    m_borders = 0;
    m_bounds = Rect<float>(LEFT_WALL_X, TOP_WALL_Y, RIGHT_WALL_X, BOTTOM_WALL_Y);
    m_gameSpeed = 0.0f;
    m_vausShips = 0;
    m_activePower = nullptr;
    m_elapsedReset = 0.0f;
    m_elapsedEndRound = 0.0f;
    m_playing = false;
    m_warpDoorOpen = false;
    m_SpawnDebrisPhase = 0;
    m_SpawnDebrisElapsed = 0.0f;
    m_explosionActive = false;
    m_PlayerStart = true;
    m_lifeTexture = 0;

#if SHOW_MOCK_GAMEPLAY
    m_mockBackground = 0;
#endif
}

void Game::OnEnter()
{
    int levelToLoad = SaveGame::round - 1;
    m_whiteFont = Engine::LoadFont("Assets/Fonts/8bitwonder.ttf", "whitefont", 32, NColor::White);
    m_orangeFont = Engine::LoadFont("Assets/Fonts/8bitwonder.ttf", "redfont", 32, NColor(224, 80, 0, 255));
    m_playerStartSFX = Engine::LoadSound("Assets/Audio/Start.wav");

    switch (levelToLoad % 4)
    {
    case 0:
        m_background = Engine::LoadTexture("Assets/Images/bg01.png");
        break;
    case 1:
        m_background = Engine::LoadTexture("Assets/Images/bg02.png");
        break;
    case 2:
        m_background = Engine::LoadTexture("Assets/Images/bg03.png");
        break;
    case 3:
        m_background = Engine::LoadTexture("Assets/Images/bg04.png");
        break;
    };

#if SHOW_MOCK_GAMEPLAY
    m_mockBackground = Engine::LoadTexture("Assets/Design/GameplayTransition.png");;
#endif

    m_borders = Engine::LoadTexture("Assets/Images/borders.png");

    m_grid.Initialize(GRID_WIDTH, GRID_HEIGHT, BLOCK_WIDTH, BLOCK_HEIGHT);
    m_grid.SetOffset(PLAYGROUND_OFFSET_X + GRID_OFFSET_X, GRID_OFFSET_Y);
    m_grid.Load(levelToLoad);

    // TODO (REVIEW) : do we need to load on every OnEnter ?
    m_warpDoor.Init("Assets/Images/warp.png", 4, 32, 102);
    m_warpDoor.AddClip("open", 1, 3, 0.2f);
    m_warpDoor.AddClip("close", 0, 1, 0.0f);
    m_warpDoor.Play("close", false);

    m_topDoorA.Init("Assets/Images/topdoor.png", 4, 48, 16);
    m_topDoorA.AddClip("open", 0, 4, 0.2f);
    m_topDoorA.AddClip("open_idle", 3, 1, 0.0f);
    m_topDoorA.AddClip("close", 4, 4, 0.2f);
    m_topDoorA.AddClip("close_idle", 7, 1, 0.0f);

    m_topDoorB.Init("Assets/Images/topdoor.png", 4, 48, 16);
    m_topDoorB.AddClip("open", 0, 4, 0.5f);
    m_topDoorB.AddClip("open_idle", 3, 1, 0.0f);
    m_topDoorB.AddClip("close", 4, 4, 0.5f);
    m_topDoorB.AddClip("close_idle", 7, 1, 0.0f);

    m_explosion.Init("Assets/Images/explosion.png", 5, 32, 32);
    m_explosion.AddClip("play", 0, 5, 0.2f);

    m_lifeTexture = Engine::LoadTexture("Assets/Images/life.png");

    m_vausShips = 2;
    m_elapsedReset = 0.0f;
    m_paddle.Initialize();
    m_paddle.OnLaserShotDelegate.Clear();
    m_paddle.OnLaserShotDelegate.Bind(this, &Game::OnLaserShot);
    m_paddle.OnExitLevelDelegate.Clear();
    m_paddle.OnExitLevelDelegate.Bind(this, &Game::OnExitLevel);
    m_warpDoorOpen = false;

    m_activePower = nullptr;

    for (Capsule* c : m_activeCapsules)
    {
        delete c;
    }
    m_activeCapsules.clear();

    for (Laser* l : m_activateLasers)
    {
        delete l;
    }
    m_activateLasers.clear();

    for (Ball* b : m_activeBalls)
    {
        delete b;
    }
    m_activeBalls.clear();

    for (Debris* d : m_activeDebris)
    {
        delete d;
    }
    m_activeDebris.clear();

    Ball* firstBall = AddBall();
    SetGameSpeed(500.0f);
    firstBall->SetAngle(SHARP_ANGLE);
    firstBall->ChangeDirection(0, -1);

    m_explosionActive = false;

    m_playerStartElapsed = 0.0f;
    m_taskMgr.Clear();
    m_taskMgr.Add(this, &Game::TaskPlayerStart);

#if SKIP_PLAYER_READY
    m_taskMgr.Clear();
    m_playing = true;
    m_PlayerStart = false;
#else
    Engine::PlaySFX(m_playerStartSFX);
    m_PlayerStart = true;
#endif
}

void Game::OnUpdate(float dt)
{
    if (Engine::GetKeyDown(KEY_DEBUG))
    {
        LOG(LL_DEBUG, "Spawn a debris");

        m_SpawnDebrisPhase = 0;
        m_SpawnDebrisElapsed = 0.0f;
        m_taskMgr.Add(this, &Game::TaskSpawnDebris);
    }

    if (m_playing)
    {
        m_grid.Update(dt);
        if (m_grid.Cleared())
        {
            m_playing = false;
            m_taskMgr.Add(this, &Game::TaskLevelCleared);
        }
        else
        {
            if (m_warpDoorOpen)
            {
                m_paddle.Update(dt, LEFT_WALL_X, static_cast<float>(SCREEN_WIDTH));
            }
            else
            {
                m_paddle.Update(dt, LEFT_WALL_X, RIGHT_WALL_X);
            }

            for (Ball* ball : m_activeBalls)
            {
                ball->Update(dt, m_bounds, m_grid);
                m_paddle.CheckCollisionWith(ball);
            }

            if (m_activateLasers.size() > 0)
            {
                std::vector<Laser*>::iterator it = m_activateLasers.begin();
                while (it != m_activateLasers.end())
                {
                    Laser* current = *it;
                    current->Update(dt);

                    bool collides = current->CheckBallCollisionWithGrid(m_grid);

                    Rect<float> transform;
                    current->GetTransform(&transform);

                    if (transform.y < TOP_WALL_Y || collides)
                    {
                        it = m_activateLasers.erase(it);
                    }
                    else
                    {
                        it++;
                    }
                }
            }

            UpdateCapsules(dt);

            if (m_warpDoorOpen)
            {
                m_warpDoor.Update(dt);
            }

            auto it = m_activeDebris.begin();
            while (it != m_activeDebris.end())
            {
                (*it)->Update(dt, m_bounds, m_grid, m_paddle);
                Rect<float> debrisTransform;
                (*it)->GetTransform(&debrisTransform);

                bool deleted = false;

#if !DEBRIS_IMMUNE_FROM_BALL
                for (Ball* ball : m_activeBalls)
                {
                    Rect<float> ballTransform;
                    ball->GetTransform(&ballTransform);

                    if (Engine::CheckRects(debrisTransform, ballTransform))
                    {
                        it = m_activeDebris.erase(it);
                        deleted = true;
                        break;
                    }
                }
#endif

                if (!deleted)
                {
                    auto laserIt = m_activateLasers.begin();
                    while (laserIt != m_activateLasers.end())
                    {
                        Rect<float> laserTransform;
                        (*laserIt)->GetTransform(&laserTransform);
                        if (Engine::CheckRects(debrisTransform, laserTransform))
                        {
                            it = m_activeDebris.erase(it);
                            laserIt = m_activateLasers.erase(laserIt);
                            deleted = true;
                            break;
                        }
                        else
                        {
                            ++laserIt;
                        }
                    }
                }

                if (!deleted)
                {
                    Rect<float> paddleTransform;
                    m_paddle.GetTransform(&paddleTransform);
                    if (Engine::CheckRects(debrisTransform, paddleTransform))
                    {
                        it = m_activeDebris.erase(it);
                        deleted = true;
                    }
                }

                if (!deleted)
                {
                    it++;
                }
                else
                {
                    // TODO (REVIEW) : Add position to animation instead of using a transform ??
                    m_explosion.Play("play", false);
                    m_explosionActive = true;
                    m_explosionTransform.Set(debrisTransform.x, debrisTransform.y, debrisTransform.w, debrisTransform.w);
                    m_taskMgr.Add(this, &Game::TaskPlayExplosion);
                }
            }

            m_topDoorB.Update(dt);
            m_explosion.Update(dt);
        }
    }

    m_taskMgr.Update(dt);
}

void Game::OnRender()
{
#if SHOW_GAME_BACKGROUND
    Engine::DrawTexture(m_background, PLAYGROUND_OFFSET_X, 0);
#endif

#if SHOW_GAME_BORDERS
    Engine::DrawTexture(m_borders, PLAYGROUND_OFFSET_X, 0);
#endif

#if SHOW_MOCK_GAMEPLAY
    Engine::DrawTexture(m_mockBackground, 0, 0, NColor(255, 255, 255, 50));
#endif

    m_warpDoor.Render({ 777.0f, 827.0f, 31.0f, 102.0f });

    m_topDoorA.Render({ 201, 0, 98.0f, 34.0f });
    m_topDoorB.Render({ 553, 0, 98.0f, 34.0f });

    m_grid.Render();
    m_paddle.Render();

    for (Ball* ball : m_activeBalls)
    {
        ball->Render();
    }

    for (Capsule* capsule : m_activeCapsules)
    {
        capsule->Render();
    }

    for (Laser* laser : m_activateLasers)
    {
        laser->Render();
    }

    for (Debris* debris : m_activeDebris)
    {
        debris->Render();
    }

    if (m_explosionActive)
    {
        m_explosion.Render(m_explosionTransform);
    }

    if (m_PlayerStart)
    {
        Engine::DrawString("PLAYER 1", m_whiteFont, 300.f, 690.0f);
        Engine::DrawString("READY", m_whiteFont, 330.f, 760.0f);
    }

    Engine::DrawString("1UP", m_orangeFont, 805.0f, 210.0f);
    Engine::DrawString(std::to_string(SaveGame::score), m_whiteFont, 833.0f, 245.0f);
    Engine::DrawString("HIGH", m_orangeFont, 810.0f, 70.0f);
    Engine::DrawString("SCORE", m_orangeFont, 835.0f, 105.0f);
    Engine::DrawString(std::to_string(SaveGame::highScore), m_whiteFont, 833.0f, 140.0f);
    Engine::DrawString("ROUND", m_orangeFont, 835.0f, 825.0f);
    Engine::DrawString(std::to_string(SaveGame::round), m_whiteFont, 930.0f, 860.0f);

    int numberOfLives = Engine::Clamp(SaveGame::life, 0, 36);
    for (int i = 0; i < numberOfLives; i++)
    {
        int y = i / 3;
        int x = i % 3;
        float offsetX = static_cast<float>(x) * 70.0f;
        float offsetY = static_cast<float>(y) * 30.0f;
        Engine::DrawTexture(m_lifeTexture, 810.0f + offsetX, 450.0f + offsetY);
    }

#if SHOW_DEBUG_GAME_BOUNDARY
    Engine::DrawLine(LEFT_WALL_X, TOP_WALL_Y, LEFT_WALL_X, BOTTOM_WALL_Y, NColor::LightGreen);
    Engine::DrawLine(RIGHT_WALL_X, TOP_WALL_Y, RIGHT_WALL_X, BOTTOM_WALL_Y, NColor::LightGreen);
    Engine::DrawLine(LEFT_WALL_X, TOP_WALL_Y, RIGHT_WALL_X, TOP_WALL_Y, NColor::LightGreen);
#endif
}

void Game::OnExit()
{
}

Ball* Game::AddBall()
{
    Ball* newBall = new Ball();
    newBall->Initialize();
    newBall->OnBlockDestroyed.Bind(this, &Game::OnBlockDestroyed);
    newBall->OnBottomReached.Bind(this, &Game::OnBottomReached);
    m_activeBalls.push_back(newBall);
    return newBall;
}

void Game::SetGameSpeed(float speed)
{
    m_gameSpeed = speed;
    for (Ball* ball : m_activeBalls)
    {
        ball->SetSpeed(m_gameSpeed);
    }
}

float Game::GetGameSpeed() const
{
    return m_gameSpeed;
}

void Game::AddVaus()
{
    m_vausShips++;
}

void Game::RemoveVaus()
{
    m_vausShips--;
}

void Game::ActivateLaser()
{
    m_taskMgr.Add<Paddle>(&m_paddle, &Paddle::TaskActivateLaser);
}

void Game::DeactivateLaser()
{
    m_taskMgr.Add<Paddle>(&m_paddle, &Paddle::TaskDeactivateLaser);
}

void Game::ExpandVaus()
{
    m_taskMgr.Add<Paddle>(&m_paddle, &Paddle::TaskExpandShip);
}


void Game::StandardVaus()
{
    m_taskMgr.Add<Paddle>(&m_paddle, &Paddle::TaskContractShip);
}

void Game::ActivateCatch(bool activate)
{
    m_paddle.SetSticky(activate);
}

void Game::OpenTheWarpDoors()
{
    m_warpDoorOpen = true;
    m_warpDoor.Play("open", true);
}

void Game::CloseTheWarpDoors()
{
    m_warpDoorOpen = false;
    m_warpDoor.Play("close", false);
}

void Game::OnBlockDestroyed(const BallEvent& ballEvent)
{
    static int capsuleCount = 0;

    int ballCount = static_cast<int>(m_activeBalls.size());
    if (Capsule::CanSpawn(ballCount))
    {
        Capsule* newCapsule = Capsule::Spawn(m_activePower, ballEvent.x, ballEvent.y);
        if (newCapsule)
        {
            capsuleCount++;
            LOG(LL_DEBUG, "Spawn a %s (Count: %d)", newCapsule->ToString().c_str(), capsuleCount);

            m_activeCapsules.push_back(newCapsule);
        }
    }
}

void Game::OnBottomReached(const BallEvent& ballEvent)
{
#if INVINSIBLE
    return;
#endif

    auto it = m_activeBalls.begin();
    while (it != m_activeBalls.end())
    {
        if (*it == ballEvent.ball)
        {
            it = m_activeBalls.erase(it);
        }
        else
        {
            ++it;
        }
    }

    LOG(LL_DEBUG, "Ball reched bottom (%d balls left)", m_activeBalls.size());
    if (m_activeBalls.size() <= 0)
    {
        SaveGame::life--;
        if (SaveGame::life <= 0)
        {
            // Game Over
            SaveGame::life = 0;
            SaveGame::CheckHighScore();
            Engine::SetState("title");
        }
        else
        {
            if (m_activePower)
            {
                m_activePower->Deactivate(this);
                m_activePower = nullptr;
            }

            m_taskMgr.Add(this, &Game::TaskResetBall);
        }
    }
}

void Game::UpdateCapsules(float dt)
{
    // TODO (REVIEW) : too much loop for the same result
    for (Capsule* capsule : m_activeCapsules)
    {
        capsule->Update(dt);

        Rect<float> capsuleTransform;
        capsule->GetTransform(&capsuleTransform);
        if (capsuleTransform.y > BOTTOM_WALL_Y + BLOCK_HEIGHT)
        {
            m_destroyedCapsules.push_back(capsule);
        }

        Rect<float> paddleTransfrom;
        m_paddle.GetTransform(&paddleTransfrom);
        if (Engine::CheckRects(
            paddleTransfrom.x, paddleTransfrom.y, paddleTransfrom.w, paddleTransfrom.h,
            capsuleTransform.x, capsuleTransform.y, capsuleTransform.w, capsuleTransform.h))
        {
            ActivatePower(capsule);
            m_destroyedCapsules.push_back(capsule);
        }
    }

    for (Capsule* capsule : m_destroyedCapsules)
    {
        std::vector<Capsule*>::iterator it = m_activeCapsules.begin();
        while (it != m_activeCapsules.end())
        {
            if (capsule == *it)
            {
                it = m_activeCapsules.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    m_destroyedCapsules.clear();
}

void Game::ActivatePower(Capsule* capsule)
{
    if (m_activePower)
    {
        m_activePower->Deactivate(this);
    }

    m_activePower = capsule;
    m_activePower->Activate(this);
}

bool Game::TaskResetBall(float dt)
{
    m_elapsedReset += dt;
    if (m_elapsedReset >= 2.0f)
    {
        m_elapsedReset = 0.0f;
        Ball* newBall = AddBall();
        SetGameSpeed(500.0f);
        newBall->SetAngle(SHARP_ANGLE);
        newBall->ChangeDirection(0, -1);
        return false;
    }

    return true;
}

bool Game::TaskLevelCleared(float dt)
{
    m_elapsedEndRound += dt;
    if (m_elapsedEndRound >= 3.0f)
    {
        SaveGame::NextRound();
        if (m_grid.LastLevel())
        {
            Engine::SetState("title");
        }
        else
        {
            Engine::SetState("intro");
            return false;
        }
    }

    return true;
}

bool Game::TaskSpawnDebris(float dt)
{
    switch (m_SpawnDebrisPhase)
    {
    case 0:
    {
        m_topDoorA.Play("open", false);
        m_SpawnDebrisPhase++;
        break;
    }

    case 1:
    {
        if (!m_topDoorA.Update(dt))
        {
            m_topDoorA.Play("open_idle", false);
            m_SpawnDebrisPhase++;
        }
        break;
    }

    case 2:
    {
        Debris* newDebris = new Debris(0);
        newDebris->Initialize();
        newDebris->SetPosition(225.0f, 10.0f);
        m_activeDebris.push_back(newDebris);
        m_SpawnDebrisElapsed = 0.0f;
        m_SpawnDebrisPhase++;
        break;
    }

    case 3:
    {
        m_SpawnDebrisElapsed += dt;
        if (m_SpawnDebrisElapsed >= 0.3f)
        {
            m_SpawnDebrisPhase++;
        }
        break;
    }

    case 4:
    {
        m_topDoorA.Play("close", false);
        m_SpawnDebrisPhase++;
        break;
    }

    case 5:
    {
        if (!m_topDoorA.Update(dt))
        {
            m_topDoorA.Play("close_idle", false);
            return false;
        }
    }
    }

    return true;
}

bool Game::TaskPlayExplosion(float dt)
{
    if (!m_explosion.Update(dt))
    {
        m_explosionActive = false;
        return false;
    }

    return true;
}

bool Game::TaskPlayerStart(float dt)
{
    m_playerStartElapsed += dt;
    if (m_playerStartElapsed >= 3)
    {
        m_PlayerStart = false;
        m_playing = true;
        return false;
    }
    return true;
}

void Game::OnLaserShot(const LaserEvent& laserEvent)
{
    LOG(LL_WARNING, "OnLaserShot called");

    Laser* laserA = new Laser(laserEvent.leftStartX, laserEvent.leftStartY, laserEvent.startV);
    laserA->Initialize();

    Laser* laserB = new Laser(laserEvent.rightStartX, laserEvent.rightStartY, laserEvent.startV);
    laserB->Initialize();

    m_activateLasers.push_back(laserA);
    m_activateLasers.push_back(laserB);
}

void Game::OnExitLevel(const PaddleEvent& paddleEvent)
{
    m_warpDoorOpen = false;
    m_playing = false;
    m_taskMgr.Add(this, &Game::TaskLevelCleared);
}
