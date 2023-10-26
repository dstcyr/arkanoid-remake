#include "Debris.h"
#include "Collision.h"
#include "Engine.h"
#include "World.h"

const float CIRCLE_LIMIT_Y = 550.0f;

Debris::Debris(int type)
{
    m_type = type;
    m_animSpeed = 0.1f;
    m_movementPhase = 0;
    m_movementTime = 0.0f;
}

void Debris::Initialize()
{
    m_movementPhase = 0;
    m_movementTime = 0.0f;
    m_transform.x = 0.0f;
    m_transform.y = 0.0f;
    m_transform.w = 60.0f;
    m_transform.h = 60.0f;
    m_velocity.x = 100.0f;
    m_velocity.y = 100.0f;

    switch (m_type)
    {
    case 0: // HAT
        m_anim.Init("Assets/Images/debris01.png", 8, 32, 32);
        m_anim.AddClip("all", 0, 8, m_animSpeed);
        break;

    case 1: // TRIANGLE
        m_anim.Init("Assets/Images/debris02.png", 7, 32, 32);
        m_anim.AddClip("all", 0, 7, m_animSpeed);
        break;

    case 2: // BUBBLE
        m_anim.Init("Assets/Images/debris03.png", 6, 32, 32);
        m_anim.AddClip("all", 0, 6, m_animSpeed);
        break;

    case 3: // CUBE
        m_anim.Init("Assets/Images/debris04.png", 8, 32, 32);
        m_anim.AddClip("all", 0, 8, m_animSpeed);
        break;
    }

    m_anim.Play("all", true);
}

void Debris::Update(float dt)
{
    m_anim.Update(dt);

    // ENTRANCE
    if (m_movementPhase == 0)
    {
        m_movementTime += dt;
        if (m_movementTime >= 0.3f)
        {
            m_movementPhase++;
        }
        else
        {
            float pY = m_transform.y + m_velocity.y * dt;
            m_transform.y = pY;
        }
    }
    // MOVEMENT
    else if (m_movementPhase == 1)
    {
        int id;
        float pY = m_transform.y;
        float pX = m_transform.x;
        bool canMoveDown = true;

        pY = m_transform.y + m_velocity.y * dt;
        if ((World::Get().CheckCollision(pX, pY, m_transform.w, m_transform.h, &id)) || 
            (pX <= LEFT_WALL_X || pX >= RIGHT_WALL_X - m_transform.w) || (pY <= TOP_WALL_Y || pY >= BOTTOM_WALL_Y))
        {
            pY = m_transform.y;
            canMoveDown = false;
        }

        if (!canMoveDown)
        {
            pX = m_transform.x + m_velocity.x * dt;
            if ((World::Get().CheckCollision(pX, pY, m_transform.w, m_transform.h, &id)) ||
                (pX <= LEFT_WALL_X || pX >= RIGHT_WALL_X - m_transform.w) || (pY <= TOP_WALL_Y || pY >= BOTTOM_WALL_Y))
            {
                pX = m_transform.x;
                m_velocity.x = -m_velocity.x;
            }
        }

        m_transform.x = pX;
        m_transform.y = pY;

        if (m_transform.y > CIRCLE_LIMIT_Y)
        {
            m_movementPhase++;
        }
    }
    // CIRCLE
    else if (m_movementPhase == 2)
    {
        m_velocity.Rotate(0.01f);

        int id;
        float pX = m_transform.x + m_velocity.x * dt;
        float pY = m_transform.y;

        if ((World::Get().CheckCollision(pX, m_transform.y, m_transform.w, m_transform.h, &id)) ||
            (pX <= LEFT_WALL_X || pX >= RIGHT_WALL_X - m_transform.w) || (pY <= TOP_WALL_Y || pY >= BOTTOM_WALL_Y))
        {
            pX = m_transform.x;
        }

        pY = m_transform.y + m_velocity.y * dt;
        if ((World::Get().CheckCollision(pX, pY, m_transform.w, m_transform.h, &id)) ||
            (pX <= LEFT_WALL_X || pX >= RIGHT_WALL_X - m_transform.w) || (pY <= TOP_WALL_Y || pY >= BOTTOM_WALL_Y))
        {
            pY = m_transform.y;
        }

        m_transform.x = pX;
        m_transform.y = pY;
    }
}

void Debris::Render()
{
    m_anim.Render(m_transform);

#if DRAW_DEBRIS_DEBUG
    Engine::DrawRect(m_transform.x, m_transform.y, m_transform.w, m_transform.h, NColor::Yellow);
    Engine::DrawLine(0.0f, CIRCLE_LIMIT_Y, SCREEN_WIDTH, CIRCLE_LIMIT_Y, NColor::Yellow);
#endif
}