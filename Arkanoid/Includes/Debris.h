#pragma once
#include "MovingObject.h"
#include "graphics/Animation.h"
#include "BallManager.h"

class Debris : public MovingObject
{
public:
    Debris(int type);
    void Initialize();
    void Update(float dt);
    void Render();

private:
    int m_type;
    float m_animSpeed;
    Animation m_anim;
    int m_movementPhase;
    float m_movementTime;
};