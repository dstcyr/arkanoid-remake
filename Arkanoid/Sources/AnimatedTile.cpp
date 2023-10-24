#include "AnimatedTile.h"
#include "Engine.h"

void AnimatedTile::Init(const std::string& filename, int frameInRows, int frameWidth, int frameHeight)
{
    anim.Init(filename, frameInRows, frameWidth, frameHeight);
    anim.AddClip("idle", 0, 1, 0.0f);
    anim.AddClip("flash", 0, 7, 0.03f);
    anim.Play("idle", false);
}

void AnimatedTile::Update(float dt)
{
    anim.Update(dt);
}

void AnimatedTile::Render(float x, float y, float w, float h)
{
    if (tileType == EMPTY_TILE) return;
    Rect<float> dst = { x, y, w, h };
    anim.Render(dst);
}

void AnimatedTile::Hit()
{
    Tile::Hit();
    if (hit > 0)
    {
        anim.Play("flash", false);
    }
}