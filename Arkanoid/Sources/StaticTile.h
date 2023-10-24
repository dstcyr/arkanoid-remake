#pragma once
#include "Tile.h"
#include "Rect.h"
#include "Config.h"

struct StaticTile final : public Tile
{
    void Render(float x, float y, float w, float h) override;

    Rect<int> source;
    size_t texture;
};