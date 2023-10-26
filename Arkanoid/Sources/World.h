#pragma once
#include "Config.h"
#include <vector>
#include "Delegate.h"

struct Tile;
class Ship;

struct BlockEvent : public Event
{
    BlockEvent() : BlockEvent(0.0f, 0.0f) {}
    BlockEvent(float x, float y) : x(x), y(y) {}
    float x = 0.0f;
    float y = 0.0f;
};

class World
{
public:
    static World& Get();

    void LoadLevel(int levelToLoad);
    void Update(float dt);
    void Render();
    void Clear();
    Ship* GetShip();

    int HitTile(int index);
    bool CheckCollision(float x, float y, float w, float h, int* id) const;
    void GetWorldPositionFromIndex(int index, int* outX, int* outY) const;
    int GetCellWidth() const;
    int GetCellHeight() const;
    bool LevelCleared();
    void KillShip();
    int GetDebrisTypeForCurrentLevel() const;
    bool IsLoaded() const { return m_loaded; }
    void ClearShip();
    void AddShip();

    CDelegate OnBlockDestroyed;

private:
    std::vector<Tile*> m_gridData;
    int m_offsetX = 0;
    int m_offsetY = 0;
    int m_width = 0;
    int m_height = 0;
    int m_cellWidth = 0;
    int m_cellHeight = 0;
    int m_totalCell = 0;
    int m_currentLevelDebris = 0;
    bool m_loaded = false;

    Ship* m_ship = nullptr;

    World() = default;
    World(const World& other) = default;
    ~World() = default;
    void AddStaticTile(int tileType);
    void AddSilverTile(int level);
    void AddGoldTile();
    void RenderGrid();
    int GetIndexFromPosition(int x, int y) const;
    void GetLocalPosition(int index, int width, int* outX, int* outY) const;
    void ToWorld(const int localX, const int localY, float* worldX, float* worldY) const;
};
