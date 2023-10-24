#pragma once
#include "Config.h"

struct Tile
{
    virtual ~Tile() = default;
    virtual void Update(float dt);
    virtual void Render(float x, float y, float w, float h);
    virtual void Hit();

    int tileType = 0;
    int hit = 0;
    int score = 0;
};