#include "Tile.h"

void Tile::Update(float dt)
{
}

void Tile::Render(float x, float y, float w, float h)
{
}

void Tile::Hit()
{
    if (tileType == EMPTY_TILE || tileType == GOLD_TILE) return;
    hit--;
}
