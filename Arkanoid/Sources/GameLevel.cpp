#include "GameLevel.h"
#include <string>
#include <sstream>
#include "Tile.h"
#include "AnimatedTile.h"
#include "StaticTile.h"
#include "Ship.h"
#include "maths/MathUtils.h"
#include "utils/tinyxml2.h"
#include "SaveGame.h"
#include "utils/Checks.h"
#include "Game.h"

using namespace tinyxml2;

int coloredBricksScore[8] = { 50, 60, 70, 80, 90, 100, 110, 120 };

GameLevel& GameLevel::Get()
{
    static GameLevel worldStaticInstance;
    return worldStaticInstance;
}

void GameLevel::LoadLevel(int levelToLoad)
{
    std::string levelFilename = "Levels/Round" + std::to_string(levelToLoad + 1) + ".tmx";

    auto& resMgr = Game::Get().Resources();
    std::vector<char> levelData = resMgr.LoadData(levelFilename);
    std::string levelDataStr;

    if (!levelData.empty())
    {
        levelDataStr = std::string(levelData.begin(), levelData.end());
    }

    tinyxml2::XMLDocument doc;
    if (doc.Parse(levelDataStr.c_str()) != XML_SUCCESS)
    {
        BX_LOG(ELogLevel::Error, "Cannot load " + levelFilename);
        return;
    }

    XMLNode* mapNode = doc.FirstChild()->NextSibling();
    XMLElement* mapElement = mapNode->ToElement();
    m_offsetX = PLAYGROUND_OFFSET_X + GRID_OFFSET_X;
    m_offsetY = GRID_OFFSET_Y;
    m_width = mapElement->IntAttribute("width");
    m_height = mapElement->IntAttribute("height");
    m_cellWidth = mapElement->IntAttribute("tilewidth");
    m_cellHeight = mapElement->IntAttribute("tileheight");
    m_currentLevelDebris = levelToLoad % 4;
    m_totalCell = m_width * m_height;

    XMLNode* layerNode = mapNode->FirstChild();
    while (layerNode)
    {
        XMLElement* layerElement = layerNode->ToElement();
        std::string valStr = layerElement->Value();
        if (valStr == "layer")
        {
            XMLNode* dataNode = layerNode->FirstChild();
            XMLElement* dataElement = dataNode->ToElement();
            const char* data = dataElement->GetText();

            std::string currentToken;
            std::istringstream inputStream(data);
            while (std::getline(inputStream, currentToken, ','))
            {
                if (!currentToken.empty())
                {
                    int tileNum = std::stoul(currentToken);
                    if (tileNum > 0 && tileNum <= 8)
                    {
                        AddStaticTile(tileNum);
                    }
                    else if (tileNum == HARD_TILE)
                    {
                        AddSilverTile(levelToLoad);
                    }
                    else if (tileNum == GOLD_TILE)
                    {
                        AddGoldTile();
                    }
                    else
                    {
                        m_gridData.push_back(nullptr);
                    }
                }
            }
        }

        layerNode = layerNode->NextSibling();
    }

    AddShip();
    m_loaded = true;
}

void GameLevel::Update(float dt)
{
    for (Tile* tile : m_gridData)
    {
        if (tile)
        {
            tile->Update(dt);
        }
    }

    BX_CHECKS(m_ship, "Invalid ship");
    m_ship->Update(dt);
}

void GameLevel::Render()
{
    RenderGrid();

    BX_CHECKS(m_ship, "Invalid ship");
    m_ship->Render();
}

void GameLevel::Clear()
{
    for (Tile* tile : m_gridData)
    {
        if (tile)
        {
            delete tile;
        }
    }

    m_gridData.clear();
    m_loaded = false;
}

void GameLevel::ClearShip()
{
    SAFE_DELETE(m_ship);
}

void GameLevel::AddShip()
{
    m_ship = new Ship();
    m_ship->Initialize();
}

void GameLevel::ToLocal(const float worldX, const float worldY, int* localX, int* localY) const
{
    *localX = static_cast<int>((worldX - m_offsetX) / m_cellWidth);
    *localY = static_cast<int>((worldY - m_offsetY) / m_cellHeight);
}

int GameLevel::GetTileType(int x, int y)
{
    if(x < 0 || y < 0 || x >= m_width || y >= m_height)
    {
        return -1;
    }

    int idx = GetIndexFromPosition(x, y);
    if(m_gridData[idx] != nullptr)
    {
        return m_gridData[idx]->tileType;
    }

    return EMPTY_TILE;
}

bool GameLevel::GetHeight() const
{
    return m_height;
}

bool GameLevel::GetWidth() const
{
    return m_width;
}

void GameLevel::ToWorld(const int localX, const int localY, float* worldX, float* worldY) const
{
    *worldX = static_cast<float>(localX * m_cellWidth) + m_offsetX;
    *worldY = static_cast<float>(localY * m_cellHeight) + m_offsetY;
}

bool GameLevel::LevelCleared()
{
    for (int i = 0; i < m_width; i++)
    {
        for (int j = 0; j < m_height; j++)
        {
            int idx = GetIndexFromPosition(i, j);
            if (m_gridData[idx] != nullptr && m_gridData[idx]->tileType != EMPTY_TILE && m_gridData[idx]->tileType != GOLD_TILE)
            {
                return false;
            }
        }
    }

    m_loaded = false;
    return true;
}

void GameLevel::KillShip()
{
    if (m_ship)
    {
        m_ship->Die();
    }
}

int GameLevel::GetDebrisTypeForCurrentLevel() const
{
    return m_currentLevelDebris;
}

Ship* GameLevel::GetShip()
{
    return m_ship;
}

int GameLevel::HitTile(int index)
{
    Tile* tile = m_gridData[index];

    if (tile->tileType == EMPTY_TILE)
    {
        return 1000;
    }

    tile->Hit();
    if (tile->tileType == GOLD_TILE)
    {
        return 1000;
    }

    if (tile->hit <= 0)
    {
        tile->tileType = 0;
        SaveGame::score += tile->score;
    }

    if (tile->hit <= 0)
    {
        int x, y;
        GetWorldPositionFromIndex(index, &x, &y);
        OnBlockDestroyed.Invoke<BlockEvent>(static_cast<float>(x), static_cast<float>(y));
    }

    return tile->hit;
}

bool GameLevel::CheckCollision(float x, float y, float w, float h, int* id) const
{
    int closestId = -1;
    float closestDistance = 5000.0f;

    for (int i = 0; i < m_width; i++)
    {
        for (int j = 0; j < m_height; j++)
        {
            int idx = GetIndexFromPosition(i, j);
            if (m_gridData[idx] != nullptr && m_gridData[idx]->tileType != EMPTY_TILE)
            {
                float worldX, worldY;
                ToWorld(i, j, &worldX, &worldY);
                float fW = static_cast<float>(m_cellWidth);
                float fH = static_cast<float>(m_cellHeight);
                if (MathUtils::CheckRects(x, y, w, h, worldX, worldY, fW, fH))
                {
                    float distance = MathUtils::DistanceBetweenRectangles(x, y, w, h, worldX, worldY, fW, fH);
                    if (distance < closestDistance)
                    {
                        closestDistance = distance;
                        closestId = idx;
                    }
                }
            }
        }
    }

    *id = closestId;
    return closestId >= 0;
}

void GameLevel::GetWorldPositionFromIndex(int index, int* outX, int* outY) const
{
    int localX, localY;
    GetLocalPosition(index, m_width, &localX, &localY);
    *outX = localX * m_cellWidth + m_offsetX;
    *outY = localY * m_cellHeight + m_offsetY;
}

int GameLevel::GetCellWidth() const
{
    return m_cellWidth;
}

int GameLevel::GetCellHeight() const
{
    return m_cellHeight;
}

void GameLevel::AddStaticTile(int tileType)
{
    auto& graphics = Game::Get().Graphics();

    // Color walls
    //     - white - 50
    //     - orange - 60
    //     - light blue - 70
    //     - green - 80
    //     - red - 90
    //     - blue - 100
    //     - pink - 110
    //     - yellow - 120
    StaticTile* staticTile = new StaticTile();
    int x, y;
    int dataid = tileType - 1;
    y = dataid / 4;
    x = dataid % 4;

    staticTile->source.x = x * BLOCK_WIDTH;
    staticTile->source.y = y * BLOCK_HEIGHT;
    staticTile->source.width = BLOCK_WIDTH;
    staticTile->source.height = BLOCK_HEIGHT;
    staticTile->tileType = tileType;
    staticTile->hit = 1;
    staticTile->score = coloredBricksScore[dataid];
    staticTile->texture = graphics.LoadTexture("Images/blocks.png");
    m_gridData.push_back(staticTile);
}

void GameLevel::AddSilverTile(int level)
{
    // Silver walls - 50 points times the number of the round.
    // The number of energy ball hits required to break the
    // hard walls follows :
    //      - 2 times - 1st to 8th round
    //      - 3 times - 9th to 16th round
    //      - 4 times - 17th to 24th round
    //      - 5 times - 25th to 32nd round
    AnimatedTile* animTile = new AnimatedTile();

    int round = level + 1;
    animTile->Init("Images/silver.png", 7, BLOCK_WIDTH, BLOCK_HEIGHT);
    animTile->tileType = HARD_TILE;
    animTile->hit = static_cast<int>(std::floor((round - 1) / 8) + 2);
    animTile->score = 50 * round;
    m_gridData.push_back(animTile);
}

void GameLevel::AddGoldTile()
{
    // Gold walls are indestructible.
    AnimatedTile* animTile = new AnimatedTile();

    animTile->Init("Images/gold.png", 7, BLOCK_WIDTH, BLOCK_HEIGHT);
    animTile->tileType = GOLD_TILE;
    animTile->hit = 0;
    animTile->score = 0;
    m_gridData.push_back(animTile);
}

void GameLevel::RenderGrid()
{
    for (int y = 0; y < m_height; y++)
    {
        for (int x = 0; x < m_width; x++)
        {
            int idx = GetIndexFromPosition(x, y);
            if (m_gridData[idx] != nullptr && m_gridData[idx]->tileType != EMPTY_TILE)
            {
                m_gridData[idx]->Render(
                    static_cast<float>(x * m_cellWidth + m_offsetX),
                    static_cast<float>(y * m_cellHeight + m_offsetY),
                    static_cast<float>(m_cellWidth),
                    static_cast<float>(m_cellHeight)
                );
            }
            else
            {
#if SHOW_DEBUG_GRID
                Engine::DrawRect(x * m_cellWidth + m_offsetX, y * m_cellHeight + m_offsetY, m_cellWidth, m_cellHeight, NColor::DarkBrown);
#endif
            }
        }
    }
}

int GameLevel::GetIndexFromPosition(int x, int y) const
{
    return x + m_width * y;
}

void GameLevel::GetLocalPosition(int index, int width, int* outX, int* outY) const
{
    *outY = static_cast<int>(index) / width;
    *outX = static_cast<int>(index) - width * *outY;
}
