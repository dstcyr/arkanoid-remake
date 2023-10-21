#pragma once
#include "Config.h"
#include "Animation.h"
#include <vector>

//struct Block
//{
//    int type = 0;
//    int hit = 0;
//    int srcX = 0;
//    int srcY = 0;
//    int width = BLOCK_WIDTH;
//    int height = BLOCK_HEIGHT;
//    int score = 0;
//};

struct Tile
{
    virtual ~Tile() = default;

    int type = 0;
    int hit = 0;
    int score = 0;

    virtual void Update(float dt) {}
    virtual void Render(float x, float y, float w, float h) {}
    virtual void Hit();
};

struct StaticTile : public Tile
{
    Rect<int> source;
    size_t m_texture;

    void Render(float x, float y, float w, float h) override;
};

struct AnimatedTile : public Tile
{
    void Init(const std::string& filename, int frameInRows, int frameWidth, int frameHeight);
    void Update(float dt) override;
    void Render(float x, float y, float w, float h) override;
    void Hit() override;

private:
    Animation anim;
};

class Grid
{
public:
    Grid();
    void Initialize(int w, int h, int cellW, int cellH);
    void SetOffset(int x, int y);
    void Update(float dt);
    void Render();
    void Load(int level);
    void Clear();
    int GetCellWidth() const;
    int GetCellHeight() const;
    bool CheckCollision(float x, float y, float w, float h, int* id) const;
    void GetWorldPositionFromIndex(int index, int* outX, int* outY) const;
    bool Hit(int idx);
    bool Cleared();
    bool LastLevel();

private:
    int m_offsetX;
    int m_offsetY;
    int m_width;
    int m_height;
    int m_cellWidth;
    int m_cellHeight;
    int m_totalCell;
    std::vector<Tile*> m_gridData;

    int GetIndexFromPosition(int x, int y) const;
    void GetLocalPosition(int index, int width, int* outX, int* outY) const;
    void ToWorld(const int localX, const int localY, float* worldX, float* worldY) const;
    void ToLocal(const float worldX, const float worldY, int* localX, int* localY) const;
};