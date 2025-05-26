#include "StaticTile.h"
#include "maths/Rect.h"

#include "Game.h"
void StaticTile::Render(float x, float y, float w, float h)
{
    if (tileType == EMPTY_TILE) return;

    auto& graphics = Game::Get().Graphics();

    Rect<float> dst = { x, y, w, h };
    graphics.DrawImage(texture, source, dst, 0.0F, false, false, 255, 255, 255, 255);
}

