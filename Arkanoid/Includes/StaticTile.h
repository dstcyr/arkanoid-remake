#pragma once
#include "Tile.h"
#include "Config.h"
#include "maths/Rect.h"

struct StaticTile final : public Tile
{
    void Render(float x, float y, float w, float h) override;

    Rect<int> source;
    size_t texture;
};