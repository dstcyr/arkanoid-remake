#include "Gameplay.h"
#include "SaveGame.h"
#include "World.h"
#include "Ship.h"
#include "Game.h"
#include "Delegate.h"

void Gameplay::Enter()
{
    BX_LOG(ELogLevel::Log, "Game::OnEnter");
    int levelToLoad = SaveGame::round - 1;
    LoadUIElements(levelToLoad);
    LoadDoors();

    if (!World::Get().IsLoaded())
    {
        World::Get().LoadLevel(levelToLoad);
    }

    World::Get().OnBlockDestroyed.Bind(this, &Gameplay::OnBlockDestroyed);
    World::Get().AddShip();
    World::Get().GetShip()->OnLaserShotDelegate.Bind(this, &Gameplay::OnLaserShot);

    m_ballMgr.Initialize();
    m_ballMgr.OnBottomReached.Bind(this, &Gameplay::OnBottomReached);
    m_ballMgr.FirstBallOfTheRound();

    m_powerMgr.OnActivateSlowPower.Bind(this, &Gameplay::OnActivateSlowPower);
    m_powerMgr.OnActivateDistruptPower.Bind(this, &Gameplay::OnActivateDisruptPower);
    m_powerMgr.OnActivateBreakPower.Bind(this, &Gameplay::OnActivateBreakPower);
    m_powerMgr.OnActivateCatchPower.Bind(this, &Gameplay::OnActivateCatchPower);

    m_laserMgr.Initialize();
    m_debrisMgr.Initialize();

    m_levelEnded = false;
    m_bottomReached = false;

    auto& audio = Game::Get().Audio();

    m_warpSfx = audio.LoadAudio("Audio/warp.wav");
    audio.SetVolume(m_warpSfx, 40);

    m_PlayerDeathSfx = audio.LoadAudio("Audio/death.wav");

    m_introTheme = audio.LoadAudio("Audio/round.wav");
    audio.StopMusic();
    audio.PlaySFX(m_introTheme);
    m_PlayerStart = true;
    m_playerStartElapsed = 0.0f;
    m_ballMgr.Update(0.0f);
}

void Gameplay::Update(float dt)
{
    auto& world = Game::Get().World();

    if (m_PlayerStart)
    {
        m_playerStartElapsed += dt;
        if (m_playerStartElapsed >= 3)
        {
            m_playerStartElapsed = 0.0f;
            m_PlayerStart = false;
        }
    }
    else if (m_levelEnded)
    {
        m_levelEndElapsed += dt;
        if (m_levelEndElapsed >= 3.0f)
        {
            m_powerMgr.Clear();
            SaveGame::NextRound();
            World::Get().Clear();

            if (SaveGame::round > LAST_LEVEL)
            {
                world.ChangeState("menu");
            }
            else
            {
                world.ChangeState("intro");
            }
        }
    }
    else if (m_bottomReached)
    {
        World::Get().Update(dt);

        m_bottomReachedElapsed += dt;
        if (m_bottomReachedElapsed > 3)
        {
            m_bottomReached = false;
            if (m_ballMgr.Count() <= 0)
            {
                SaveGame::life--;
                if (SaveGame::life <= 0)
                {
                    // Game Over
                    SaveGame::life = 0;
                    SaveGame::CheckHighScore();
                    world.ChangeState("menu");
                }
                else
                {
                    m_powerMgr.Clear();
                    world.ChangeState("intro");
                }
            }
        }
    }
    else
    {
        World::Get().Update(dt);
        if (World::Get().LevelCleared())
        {
            m_levelEndElapsed = 0.0f;
            m_levelEnded = true;
        }
        else
        {
            m_powerMgr.Update(dt);
            m_laserMgr.Update(dt);
            m_debrisMgr.Update(dt);
            m_ballMgr.Update(dt);
            m_debrisMgr.CheckCollisions(m_ballMgr);
            m_debrisMgr.CheckCollisions(m_laserMgr);

            if (m_warpDoorOpen)
            {
                m_warpDoor.Update(dt);
            }
        }
    }
}

void Gameplay::Draw()
{
    auto& graphics = Game::Get().Graphics();

    graphics.DrawImage(m_background, PLAYGROUND_OFFSET_X, 0, 255, 255, 255, 255);
    graphics.DrawImage(m_borders, PLAYGROUND_OFFSET_X, 0, 255, 255, 255, 255);

    m_warpDoor.Draw({ 777.0f, 827.0f, 31.0f, 102.0f });

    World::Get().Render();
    m_ballMgr.Render();
    m_powerMgr.Render();
    m_laserMgr.Render();
    m_debrisMgr.Render();

    // HACK: Hide the ship behind a black square when warping out of the level
    graphics.FillRect(807.0f, 827.0f, 100.0f, 100.0f, 0, 0, 0, 255);
    RenderUI();
}

void Gameplay::Exit()
{
    m_warpDoorOpen = false;

    World::Get().OnBlockDestroyed.Clear();
    World::Get().GetShip()->OnLaserShotDelegate.Clear();
    World::Get().ClearShip();

    m_ballMgr.OnBottomReached.Clear();

    m_powerMgr.OnActivateSlowPower.Clear();
    m_powerMgr.OnActivateDistruptPower.Clear();
    m_powerMgr.OnActivateBreakPower.Clear();

    m_ballMgr.Clear();
    m_debrisMgr.Clear();
    m_laserMgr.Clear();
    m_powerMgr.Clear();
}

void Gameplay::OnWarpedOut()
{
    auto& audio = Game::Get().Audio();

    m_levelEndElapsed = 0.0f;
    m_levelEnded = true;
    audio.PlaySFX(m_warpSfx);
}

void Gameplay::ChooseBackground(int level)
{
    auto& graphics = Game::Get().Graphics();

    m_borders = graphics.LoadTexture("Images/borders.png");

    switch (level % 4)
    {
    case 0:
        m_background = graphics.LoadTexture("Images/bg01.png");
        break;
    case 1:
        m_background = graphics.LoadTexture("Images/bg02.png");
        break;
    case 2:
        m_background = graphics.LoadTexture("Images/bg03.png");
        break;
    case 3:
        m_background = graphics.LoadTexture("Images/bg04.png");
        break;
    };
}

void Gameplay::LoadUIElements(int Level)
{
    auto& graphics = Game::Get().Graphics();

    ChooseBackground(Level);
    m_whiteFont = graphics.LoadFont("Fonts/8bitwonder.ttf", 32);
    m_orangeFont = graphics.LoadFont("Fonts/8bitwonder.ttf", 32);
    m_lifeTexture = graphics.LoadTexture("Images/life.png");
}

void Gameplay::RenderUI()
{
    auto& graphics = Game::Get().Graphics();

    if (m_PlayerStart)
    {
        graphics.DrawString("PLAYER 1", m_whiteFont, 300.f, 690.0f, 255, 255, 255, 255);
        graphics.DrawString("READY", m_whiteFont, 330.f, 760.0f, 255, 255, 255, 255);
    }

    graphics.DrawString("1UP", m_orangeFont, 805.0f, 210.0f, 255, 255, 255, 255);
    graphics.DrawString(std::to_string(SaveGame::score), m_whiteFont, 833.0f, 245.0f, 255, 255, 255, 255);
    graphics.DrawString("HIGH", m_orangeFont, 810.0f, 70.0f, 255, 255, 255, 255);
    graphics.DrawString("SCORE", m_orangeFont, 835.0f, 105.0f, 255, 255, 255, 255);
    graphics.DrawString(std::to_string(SaveGame::highScore), m_whiteFont, 833.0f, 140.0f, 255, 255, 255, 255);
    graphics.DrawString("ROUND", m_orangeFont, 835.0f, 825.0f, 255, 255, 255, 255);
    graphics.DrawString(std::to_string(SaveGame::round), m_whiteFont, 930.0f, 860.0f, 255, 255, 255, 255);

    int numberOfLives = MathUtils::Clamp(SaveGame::life, 0, 36);
    for (int i = 0; i < numberOfLives; i++)
    {
        int y = i / 3;
        int x = i % 3;
        float offsetX = static_cast<float>(x) * 70.0f;
        float offsetY = static_cast<float>(y) * 30.0f;
        graphics.DrawImage(m_lifeTexture, 810.0f + offsetX, 450.0f + offsetY, 255, 255, 255, 255);
    }
}

void Gameplay::OnBlockDestroyed(const BlockEvent& blockEvent)
{
    BX_LOG(ELogLevel::Log, "Block destroyed");

    int ballCount = m_ballMgr.Count();
    m_powerMgr.TrySpawnCapsule(ballCount, blockEvent.x, blockEvent.y, m_warpDoorOpen);
}

void Gameplay::OnBottomReached(const BallEvent& ballEvent)
{
#if GOD_MODE
    return;
#endif

    auto& audio = Game::Get().Audio();

    if (m_ballMgr.Count() <= 1)
    {
        m_bottomReached = true;
        m_bottomReachedElapsed = 0.0f;
        World::Get().KillShip();

        audio.PlaySFX(m_PlayerDeathSfx);
    }
}

void Gameplay::OnActivateSlowPower(const PowerEvent& powerEvent)
{
    if (powerEvent.Activate)
    {
        m_ballMgr.SlowDown();
    }
}

void Gameplay::OnActivateDisruptPower(const PowerEvent& powerEvent)
{
    if (powerEvent.Activate)
    {
        m_ballMgr.SpawnBall(2);
    }
}

void Gameplay::OnActivateBreakPower(const PowerEvent& powerEvent)
{
    if (powerEvent.Activate)
    {
        m_warpDoorOpen = true;
        m_warpDoor.Play("open", true);
    }
}

void Gameplay::OnActivateCatchPower(const PowerEvent& powerEvent)
{
    if (powerEvent.Activate)
    {
        m_ballMgr.CatchingBall(true);
    }
    else
    {
        m_ballMgr.CatchingBall(false);
    }
}

void Gameplay::OnLaserShot(const LaserEvent& laserEvent)
{
    m_laserMgr.SpawnLaser(laserEvent.leftStartX, laserEvent.leftStartY,
        laserEvent.rightStartX, laserEvent.rightStartY);
}

void Gameplay::LoadDoors()
{
    m_warpDoorOpen = false;
    m_warpDoor.Load("Images/warp.png");
    m_warpDoor.Init(4, 32, 102);
    m_warpDoor.AddClip("open", 1, 3, 0.2f);
    m_warpDoor.AddClip("close", 0, 1, 0.0f);
    m_warpDoor.Play("close", false);
}