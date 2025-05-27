#pragma once
#include "Tile.h"
#include "graphics/Animation.h"
#include "Config.h"
#include <string>

struct AnimatedTile final : public Tile
{
    void Init(const std::string& filename, int frameInRows, int frameWidth, int frameHeight);
    void Update(float dt) override;
    void Render(float x, float y, float w, float h) override;
    void Hit() override;

private:
    Animation anim;
};