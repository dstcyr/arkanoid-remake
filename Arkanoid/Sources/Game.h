#pragma once
#include "Config.h"
#include "StateMachine.h"
#include "Rect.h"
#include "Grid.h"
#include "Ball.h"
#include "Paddle.h"
#include "TaskManager.h"

class Capsule;
class Laser;
class Debris;

class Game : public IState
{
public:
    Game();
    void OnEnter() override;
    void OnUpdate(float dt) override;
    void OnRender() override;
    void OnExit() override;

    Ball* AddBall();
    void SetGameSpeed(float speed);
    float GetGameSpeed() const;
    void AddVaus();
    void RemoveVaus();
    void ActivateLaser();
    void DeactivateLaser();
    void ExpandVaus();
    void StandardVaus();
    void ActivateCatch(bool activate);
    void OpenTheWarpDoors();
    void CloseTheWarpDoors();

private:
    size_t m_whiteFont;
    size_t m_orangeFont;
    size_t m_background;
    size_t m_borders;
    Rect<float> m_bounds;
    Grid m_grid;
    Paddle m_paddle;
    std::vector<Capsule*> m_activeCapsules;
    std::vector<Capsule*> m_destroyedCapsules;
    std::vector<Laser*> m_activateLasers;
    std::vector<Ball*> m_activeBalls;
    std::vector<Debris*> m_activeDebris;
    float m_gameSpeed;
    Capsule* m_activePower;
    int m_vausShips;
    CTaskManager m_taskMgr;
    float m_elapsedReset;
    float m_elapsedEndRound;
    bool m_playing;
    Animation m_warpDoor;
    Animation m_topDoorA;
    Animation m_topDoorB;
    Animation m_explosion;
    bool m_warpDoorOpen;
    int m_SpawnDebrisPhase;
    float m_SpawnDebrisElapsed;
    Rect<float> m_explosionTransform;
    bool m_explosionActive;
    float m_playerStartElapsed;
    size_t m_playerStartSFX;
    bool m_PlayerStart;
#if SHOW_MOCK_GAMEPLAY
    size_t m_mockBackground;
#endif
    size_t m_lifeTexture;

    void OnBlockDestroyed(const BallEvent& ballEvent);
    void OnBottomReached(const BallEvent& ballEvent);
    void OnLaserShot(const LaserEvent& laserEvent);
    void OnExitLevel(const PaddleEvent& paddleEvent);
    void UpdateCapsules(float dt);
    void ActivatePower(Capsule* capsule);
    bool TaskResetBall(float dt);
    bool TaskLevelCleared(float dt);
    bool TaskSpawnDebris(float dt);
    bool TaskPlayExplosion(float dt);
    bool TaskPlayerStart(float dt);
};