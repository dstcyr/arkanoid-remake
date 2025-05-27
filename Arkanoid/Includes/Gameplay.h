#pragma once
#include "Config.h"
#include <vector>
#include "Ball.h"
#include "BallManager.h"
#include "PowerManager.h"
#include "GameLevel.h"
#include "Ship.h"
#include "LaserManager.h"
#include "DebrisManager.h"
#include "graphics/Animation.h"
#include "screens/ScreenState.h"

class Gameplay : public ScreenState
{
public:
    void Enter() override;
    void Update(float dt) override;
    void Draw() override;
    void Exit() override;

    bool WarpDoorOpenned() const { return m_warpDoorOpen; }

    void OnWarpedOut();
private:
    size_t m_whiteFont;
    size_t m_orangeFont;
    size_t m_background;
    size_t m_borders;
    size_t m_lifeTexture;
    size_t m_warpSfx;
    size_t m_PlayerDeathSfx;
    size_t m_introTheme = 0;
    bool m_warpDoorOpen;
    bool m_levelEnded;
    float m_levelEndElapsed;
    bool m_bottomReached;
    float m_bottomReachedElapsed;
    bool m_PlayerStart;
    float m_playerStartElapsed;
    Animation m_warpDoor;

    BallManager m_ballMgr;
    PowerManager m_powerMgr;
    LaserManager m_laserMgr;
    DebrisManager m_debrisMgr;

    void ChooseBackground(int level);
    void LoadUIElements(int level);
    void RenderUI();

    void OnBlockDestroyed(const BlockEvent& blockEvent);
    void OnBottomReached(const BallEvent& ballEvent);
    void OnActivateSlowPower(const PowerEvent& powerEvent);
    void OnActivateDisruptPower(const PowerEvent& powerEvent);
    void OnActivateBreakPower(const PowerEvent& powerEvent);
    void OnActivateCatchPower(const PowerEvent& powerEvent);
    void OnLaserShot(const LaserEvent& laserEvent);
    void LoadDoors();
};
