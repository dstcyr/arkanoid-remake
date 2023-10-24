#include "StaticTile.h"
#include "Engine.h"

void StaticTile::Render(float x, float y, float w, float h)
{
    if (tileType == EMPTY_TILE) return;
    Rect<float> dst = { x, y, w, h };
    Engine::DrawTexture(texture, source, dst, 0.0, false, false, NColor::White);
}

