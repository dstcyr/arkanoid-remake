#include "AnimatedTile.h"

void AnimatedTile::Init(const std::string& filename, int frameInRows, int frameWidth, int frameHeight)
{
    anim.Load(filename);
    anim.Init(frameInRows, frameWidth, frameHeight);
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
    anim.Draw(x, y, w, h, 0.0F);
}

void AnimatedTile::Hit()
{
    Tile::Hit();
    if (hit > 0)
    {
        anim.Play("flash", false);
    }
}