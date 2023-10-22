#pragma once
#include "Config.h"
#include "Rect.h"
#include "Animation.h"
#include <string>

class Game;

// Some walls contain power CAPSULES.Each time you capture one you'll score 100
// points and gain a super power.
class Capsule
{
public:
    Capsule();
    Capsule(float x, float y);
    virtual ~Capsule() = default;
    virtual int GetId() const { return -1; }
    virtual void Update(float dt);
    virtual void Render();
    void GetTransform(Rect<float>* transform);

    static Capsule* Spawn(Capsule* activePower, float x, float y);
    static bool CanSpawn(int ballCount);

    virtual std::string ToString() = 0;

    virtual void Init();
    virtual void Activate(Game* game) = 0;
    virtual void Deactivate(Game* game);

    void OnActivated();
    void OnDeactivated();
    bool IsActive();

protected:
    Rect<float> m_transform;
    float m_velocity;
    Animation m_animation;
    bool m_activated;
};

// 0 : pink - opens warp escape - advance to next round
//        {   B   }
// Collect the violet capsule to create a "break out" exit on the right side of the
// stage.Passing through this exit will cause you to advance to the next stage 
// immediately, as well as earn a 10, 000 point bonus.
class BreakCapsule : public Capsule
{
public:
    BreakCapsule();
    BreakCapsule(float x, float y);

    static int GetCapsuleId() { return 0; }
    int GetId() const override { return BreakCapsule::GetCapsuleId(); }
    void Render() override;
    std::string ToString() override;
    void Activate(Game* game) override;
    void Deactivate(Game* game) override;
    void Init() override;
};

// 1: yellow / green - catch the ball and release it
//        {   C   }
// Collect the green capsule to gain the catch ability. When the ball hits the Vaus, it
// will stick to the surface. Press the Fire button to release the ball. The ball will
// automatically release after a certain period of time has passed.
class CatchCapsule : public Capsule
{
public:
    CatchCapsule();
    CatchCapsule(float x, float y);

    static int GetCapsuleId() { return 1; }
    int GetId() const override { return CatchCapsule::GetCapsuleId(); }
    void Render() override;
    std::string ToString() override;
    void Activate(Game* game) override;
    void Deactivate(Game* game) override;
    void Init() override;
};

// 2: light blue - splits energy ball into 3 particles
//        {   D   }
// Collect the cyan capsule to cause the ball to split into three instances of itself.
// All three balls can be kept aloft. There is no penalty for losing the first two balls.
// No colored capsules will fall as long as there is more than one ball in play. This is 
// the only power up that, while in effect, prevents other power ups from falling.
class DisruptCapsule : public Capsule
{
public:
    DisruptCapsule();
    DisruptCapsule(float x, float y);

    static int GetCapsuleId() { return 2; }
    int GetId() const override { return DisruptCapsule::GetCapsuleId(); }
    void Render() override;
    std::string ToString() override;
    void Activate(Game* game) override;
    void Init() override;
};

// 3: dark blue / purple - widens the Vaus controller
//        {   E   }
// Collect the blue capsule to extend the width of the Vaus.
class ExpandCapsule : public Capsule
{
public:
    ExpandCapsule();
    ExpandCapsule(float x, float y);

    static int GetCapsuleId() { return 3; }
    int GetId() const override { return ExpandCapsule::GetCapsuleId(); }
    void Render() override;
    std::string ToString() override;
    void Activate(Game* game) override;
    void Deactivate(Game* game) override;
    void Init() override;
};

// 4: red - increase to Vaus laser fire power
//        {   L   }
// Collect the red capsule to transform the Vaus into its Laser-firing configuration. In
// this form, you can fire lasers at the top of the screen by pushing the fire button.
// Lasers can be used against every brick except Gold bricks, and against enemies. Silver
// bricks can only be destroyed by lasers when they are hit the required number of times.
class LaserCapsule : public Capsule
{
public:
    LaserCapsule();
    LaserCapsule(float x, float y);

    static int GetCapsuleId() { return 4; }
    int GetId() const override { return LaserCapsule::GetCapsuleId(); }
    void Render() override;
    std::string ToString() override;
    void Activate(Game* game) override;
    void Deactivate(Game* game) override;
    void Init() override;
};

// 5: grey - awards an extra Vaus
//        {   P   }
// Collect the gray capsule to earn an extra Vaus.
class PlayerCapsule : public Capsule
{
public:
    PlayerCapsule();
    PlayerCapsule(float x, float y);

    static int GetCapsuleId() { return 5; }
    int GetId() const override { return PlayerCapsule::GetCapsuleId(); }
    void Render() override;
    std::string ToString() override;
    void Activate(Game* game) override;
    void Init() override;
};

// 6: orange - slows movement of ball
//        {   S   }
// Collect the orange capsule to slow the velocity at which the ball moves. 
// Collecting multiple orange capsules will have a cumulative effect and the ball velocity can
// become extremely slow. However, the ball velocity will gradually increase as it
// bounces and destroys bricks. The velocity may sometimes suddenly increase with little
// warning.
class SlowCapsule : public Capsule
{
public:
    SlowCapsule();
    SlowCapsule(float x, float y);

    static int GetCapsuleId() { return 6; }
    int GetId() const override { return SlowCapsule::GetCapsuleId(); }
    void Render() override;
    std::string ToString() override;
    void Activate(Game* game) override;
    void Deactivate(Game* game) override;
    void Init() override;
};
