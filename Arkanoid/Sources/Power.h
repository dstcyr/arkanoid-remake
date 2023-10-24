#pragma once
#include "MovingObject.h"
#include "Animation.h"

class Power : public MovingObject
{
public:
    Power();
    Power(float x, float y);
    virtual ~Power() = default;
    virtual void Update(float dt);
    virtual void Render();
    virtual void Init();
    virtual void Activate();
    virtual void Deactivate();
    virtual std::string ToString() = 0;
    virtual int GetInstanceID() const = 0;

protected:
    Animation m_animation;
};