#pragma once
#include "Animation.h"
#include "Rect.h"
#include "Grid.h"
#include "Paddle.h"
#include "Vec2D.h"

class Debris
{
public:
    Debris(int type);
    void Initialize();
    void Update(float dt, const Rect<float>& bounds, const Grid& grid, const Paddle& paddle);
    void Render();
    void SetPosition(float x, float y);

    float GetX() const { return m_transform.x; }
    float GetY() const { return m_transform.y; }
    void GetTransform(Rect<float>* transform);

private:
    int m_type;
    float m_animSpeed;
    Animation m_anim;
    Rect<float> m_transform;
    Vec2D m_velocity;
};