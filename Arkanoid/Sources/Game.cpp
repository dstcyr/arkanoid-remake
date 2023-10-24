#include "Game.h"
#include "SaveGame.h"
#include "Engine.h"
#include "World.h"
#include "Ship.h"
#include "Log.h"
#include "MathUtils.h"

void Game::OnEnter()
{
    int levelToLoad = SaveGame::round - 1;
    LoadUIElements(levelToLoad);
    LoadDoors();

    World::Get().LoadLevel(levelToLoad);
    World::Get().OnBlockDestroyed.Bind(this, &Game::OnBlockDestroyed);
    World::Get().GetShip()->OnLaserShotDelegate.Bind(this, &Game::OnLaserShot);

    m_ballMgr.OnBallReachedBottom.Bind(this, &Game::OnBottomReached);
    m_ballMgr.SpawnBall();

    m_powerMgr.OnActivateSlowPower.Bind(this, &Game::OnActivateSlowPower);
    m_powerMgr.OnActivateDistruptPower.Bind(this, &Game::OnActivateDisruptPower);
    m_powerMgr.OnActivateBreakPower.Bind(this, &Game::OnActivateBreakPower);

    m_levelEnded = false;
}

void Game::OnUpdate(float dt)
{
    if (m_levelEnded)
    {
        m_levelEndElapsed += dt;
        if (m_levelEndElapsed >= 3.0f)
        {
            m_powerMgr.Clear();
            SaveGame::NextRound();
            if (SaveGame::round > LAST_LEVEL)
            {
                Engine::SetState("menu");
            }
            else
            {
                Engine::SetState("intro");
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
            m_ballMgr.Update(dt);
            m_powerMgr.Update(dt);
            m_laserMgr.Update(dt);

            if (m_warpDoorOpen)
            {
                m_warpDoor.Update(dt);
            }
        }
    }
}

void Game::OnRender()
{
    Engine::DrawTexture(m_background, PLAYGROUND_OFFSET_X, 0);
    Engine::DrawTexture(m_borders, PLAYGROUND_OFFSET_X, 0);

    m_warpDoor.Render({ 777.0f, 827.0f, 31.0f, 102.0f });

    World::Get().Render();
    m_ballMgr.Render();
    m_powerMgr.Render();
    m_laserMgr.Render();

    // HACK: Hide the ship behind a black square when warping out of the level
    Engine::FillRect(807.0f, 827.0f, 100.0f, 100.0f, NColor(0, 0, 0, 255));
    RenderUI();
}

void Game::OnExit()
{
    m_warpDoorOpen = false;

    m_ballMgr.Destroy();
    World::Get().OnBlockDestroyed.Clear();
    World::Get().Clear();
}

void Game::OnWarpedOut()
{
    LOG(LL_DEBUG, "Warped out");
}

void Game::ChooseBackground(int level)
{
    m_borders = Engine::LoadTexture("Assets/Images/borders.png");

    switch (level % 4)
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
}

void Game::LoadUIElements(int Level)
{
    ChooseBackground(Level);
    m_whiteFont = Engine::LoadFont("Assets/Fonts/8bitwonder.ttf", "whitefont", 32, NColor::White);
    m_orangeFont = Engine::LoadFont("Assets/Fonts/8bitwonder.ttf", "redfont", 32, NColor(224, 80, 0, 255));
    m_lifeTexture = Engine::LoadTexture("Assets/Images/life.png");
}

void Game::RenderUI()
{
    /// if (m_PlayerStart)
    /// {
    ///     Engine::DrawString("PLAYER 1", m_whiteFont, 300.f, 690.0f);
    ///     Engine::DrawString("READY", m_whiteFont, 330.f, 760.0f);
    /// }

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
}

void Game::OnBlockDestroyed(const BlockEvent& blockEvent)
{
    LOG(LL_DEBUG, "Block destroyed");

    int ballCount = m_ballMgr.Count();
    m_powerMgr.TrySpawnCapsule(ballCount, blockEvent.x, blockEvent.y, m_warpDoorOpen);
}

void Game::OnBottomReached(const BallEvent& ballEvent)
{
#if INVINSIBLE
    return;
#endif
    
    m_ballMgr.Remove(ballEvent.ball);

    if (m_ballMgr.Count() <= 0)
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
            m_powerMgr.Clear();
            Engine::SetState("intro");
        }
    }
}

void Game::OnActivateSlowPower(const Event& powerEvent)
{
    m_ballMgr.SlowDown();
}

void Game::OnActivateDisruptPower(const Event& powerEvent)
{
    m_ballMgr.SpawnBall(2);
}

void Game::OnActivateBreakPower(const Event& powerEvent)
{
    m_warpDoorOpen = true;
    m_warpDoor.Play("open", true);
}

void Game::OnLaserShot(const LaserEvent& laserEvent)
{
    m_laserMgr.SpawnLaser(laserEvent.leftStartX, laserEvent.leftStartY,
        laserEvent.rightStartX, laserEvent.rightStartY);
}

void Game::LoadDoors()
{
    m_warpDoorOpen = false;
    m_warpDoor.Init("Assets/Images/warp.png", 4, 32, 102);
    m_warpDoor.AddClip("open", 1, 3, 0.2f);
    m_warpDoor.AddClip("close", 0, 1, 0.0f);
    m_warpDoor.Play("close", false);
}
