#include "Grid.h"
#include "Engine.h"
#include "Log.h"
#include "MathUtils.h"
#include "Ball.h"
#include "Collision.h"
#include "MathUtils.h"
#include "LevelData.h"
#include <cmath>
#include "SaveGame.h"
#include "tinyxml2.h"
#include <sstream>

using namespace tinyxml2;

#if SHOW_DEBUG_GRID_COORD
#include <string>
size_t debugFont = 0;
#endif

int coloredBricksScore[8] = { 50, 60, 70, 80, 90, 100, 110, 120 };

Grid::Grid()
{
    m_offsetX = 0;
    m_offsetY = 0;
    m_width = 0;
    m_height = 0;
    m_cellWidth = 0;
    m_cellHeight = 0;
    m_totalCell = 0;
}

void Grid::Initialize(int w, int h, int cellW, int cellH)
{
    m_gridData.clear();
    m_offsetX = 0;
    m_offsetY = 0;
    m_width = w;
    m_height = h;
    m_cellWidth = cellW;
    m_cellHeight = cellH;
    m_totalCell = w * h;

#if SHOW_DEBUG_GRID_COORD
    debugFont = Engine::LoadFont("Assets/Fonts/8bitwonder.ttf", "debugFont", 8, NColor::White);
#endif
}

void Grid::SetOffset(int x, int y)
{
    m_offsetX = x;
    m_offsetY = y;
}

void Grid::Update(float dt)
{
    for (Tile* tile : m_gridData)
    {
        if (tile)
        {
            tile->Update(dt);
        }
    }
}

void Grid::Render()
{
    for (int y = 0; y < m_height; y++)
    {
        for (int x = 0; x < m_width; x++)
        {
            int idx = GetIndexFromPosition(x, y);
            if (m_gridData[idx] != nullptr && m_gridData[idx]->type != 0)
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

#if SHOW_DEBUG_GRID_COORD
            float worldX, worldY;
            ToWorld(x, y, &worldX, &worldY);
            std::string location = std::string("(") + std::to_string(x) + std::string(",") + std::to_string(y) + std::string(")");
            Engine::DrawString(location, debugFont, worldX, worldY);
#endif
        }
    }
}

void Grid::Load(int level)
{
    std::string levelFilename = "Assets/Levels/Round" + std::to_string(level + 1) + ".tmx";

    XMLDocument doc;
    if (doc.LoadFile(levelFilename.c_str()) == XML_SUCCESS)
    {
        XMLNode* mapNode = doc.FirstChild()->NextSibling();
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

                std::string tCurrentToken;
                std::istringstream tInput(data);

                int tX = 0;
                int tY = 0;

                // m_LayerData.push_back(std::vector<TileInfo*>(m_Width));

                while (std::getline(tInput, tCurrentToken, ','))
                {
                    if (!tCurrentToken.empty())
                    {
                        // TileInfo* tInfo = new TileInfo();

                        try
                        {
                            int tileNum = std::stoul(tCurrentToken);
                            // tInfo->HorizontalFlip = tInfo->Index & FLIPPED_HORIZONTALLY_FLAG;
                            // tInfo->VerticalFlip = tInfo->Index & FLIPPED_VERTICALLY_FLAG;
                            // tInfo->DiagonalFlip = tInfo->Index & FLIPPED_DIAGONALLY_FLAG;
                            // tInfo->Index &= ~(FLIPPED_HORIZONTALLY_FLAG | FLIPPED_VERTICALLY_FLAG | FLIPPED_DIAGONALLY_FLAG);

                            if (tileNum > 0 && tileNum <= 8)
                            {
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
                                int dataid = tileNum - 1;
                                y = dataid / 4;
                                x = dataid % 4;

                                staticTile->source.x = x * BLOCK_WIDTH;
                                staticTile->source.y = y * BLOCK_HEIGHT;
                                staticTile->source.w = BLOCK_WIDTH;
                                staticTile->source.h = BLOCK_HEIGHT;
                                staticTile->type = tileNum;
                                staticTile->hit = 1;
                                staticTile->score = coloredBricksScore[dataid];
                                staticTile->m_texture = Engine::LoadTexture("Assets/Images/blocks.png");
                                m_gridData.push_back(staticTile);
                            }
                            else if (tileNum == 9)
                            {
                                // TODO (REVIEW) : optimize code repetition here

                                // Silver walls - 50 points times the number of the round.
                                // The number of energy ball hits required to break the 
                                // hard walls follows :
                                //      - 2 times - 1st to 8th round
                                //      - 3 times - 9th to 16th round
                                //      - 4 times - 17th to 24th round
                                //      - 5 times - 25th to 32nd round
                                AnimatedTile* animTile = new AnimatedTile();

                                int round = level + 1;
                                animTile->Init("Assets/Images/silver.png", 6, BLOCK_WIDTH, BLOCK_HEIGHT);
                                animTile->type = tileNum;
                                animTile->hit = static_cast<int>(std::floor((round - 1) / 8) + 2);
                                animTile->score = 50 * round;
                                m_gridData.push_back(animTile);
                            }
                            else if (tileNum == 10)
                            {
                                // Gold walls are indestructible.
                                AnimatedTile* animTile = new AnimatedTile();

                                animTile->Init("Assets/Images/gold.png", 6, BLOCK_WIDTH, BLOCK_HEIGHT);
                                animTile->type = tileNum;
                                animTile->hit = 100;
                                animTile->score = 0;
                                m_gridData.push_back(animTile);
                            }
                            else
                            {
                                m_gridData.push_back(nullptr);
                            }
                        }
                        catch (const std::exception& exp)
                        {
                            //Debug::LogError("Corrupted map detected " + m_LayerName);
                            //Debug::LogError("Error: " + std::string(exp.what()));
                            LOG(LL_DEBUG, "Corrupted map: %s", exp.what());
                            return;
                        }

                        /// m_LayerData[tY][tX] = tInfo;
                        /// tX++;
                        /// 
                        /// if (tX >= m_Width && tY < m_Height - 1)
                        /// {
                        ///     tX = 0;
                        ///     tY++;
                        /// 
                        ///     m_LayerData.push_back(std::vector<TileInfo*>(m_Width));
                        /// }
                    }
                    else
                    {
                        // Debug::Log("Invalid map detected " + m_LayerName);
                        LOG(LL_DEBUG, "Invalid map");
                        return;
                    }
                }


            }

            layerNode = layerNode->NextSibling();
        }
    }





































    //> const std::vector<int>& data = Level::data[level];
    //> 
    //> for (int i = 0; i < m_totalCell; i++)
    //> {
    //>     int typeOfTile = data[i];
    //>     if (typeOfTile > 0 && typeOfTile <= 8)
    //>     {
    //>         // Color walls
    //>         //     - white - 50
    //>         //     - orange - 60
    //>         //     - light blue - 70
    //>         //     - green - 80
    //>         //     - red - 90
    //>         //     - blue - 100
    //>         //     - pink - 110
    //>         //     - yellow - 120
    //>         StaticTile* staticTile = new StaticTile();
    //>         int x, y;
    //>         int dataid = typeOfTile - 1;
    //>         y = dataid / 4;
    //>         x = dataid % 4;
    //> 
    //>         staticTile->source.x = x * BLOCK_WIDTH;
    //>         staticTile->source.y = y * BLOCK_HEIGHT;
    //>         staticTile->source.w = BLOCK_WIDTH;
    //>         staticTile->source.h = BLOCK_HEIGHT;
    //>         staticTile->type = typeOfTile;
    //>         staticTile->hit = 1;
    //>         staticTile->score = coloredBricksScore[dataid];
    //>         staticTile->m_texture = Engine::LoadTexture("Assets/Images/blocks.png");
    //>         m_gridData.push_back(staticTile);
    //>     }
    //>     else if (data[i] == 9)
    //>     {
    //>         // TODO : optimize code repetition here
    //> 
    //>         // Silver walls - 50 points times the number of the round.
    //>         // The number of energy ball hits required to break the 
    //>         // hard walls follows :
    //>         //      - 2 times - 1st to 8th round
    //>         //      - 3 times - 9th to 16th round
    //>         //      - 4 times - 17th to 24th round
    //>         //      - 5 times - 25th to 32nd round
    //>         AnimatedTile* animTile = new AnimatedTile();
    //> 
    //>         int round = level + 1;
    //>         animTile->Init("Assets/Images/silver.png", 6, BLOCK_WIDTH, BLOCK_HEIGHT);
    //>         animTile->type = typeOfTile;
    //>         animTile->hit = static_cast<int>(std::floor((round - 1) / 8) + 2);
    //>         animTile->score = 50 * round;
    //>         m_gridData.push_back(animTile);
    //>     }
    //>     else if (data[i] == 10)
    //>     {
    //>         // Gold walls are indestructible.
    //>         AnimatedTile* animTile = new AnimatedTile();
    //> 
    //>         animTile->Init("Assets/Images/gold.png", 6, BLOCK_WIDTH, BLOCK_HEIGHT);
    //>         animTile->type = typeOfTile;
    //>         animTile->hit = 100;
    //>         animTile->score = 0;
    //>         m_gridData.push_back(animTile);
    //>     }
    //>     else
    //>     {
    //>         m_gridData.push_back(nullptr);
    //>     }
    //> }
}

void Grid::Clear()
{
    for (Tile* tile : m_gridData)
    {
        delete tile;
    }

    m_gridData.clear();
}

void Grid::GetLocalPosition(int index, int width, int* outX, int* outY) const
{
    *outY = static_cast<int>(index) / width;
    *outX = static_cast<int>(index) - width * *outY;
}

int Grid::GetCellWidth() const
{
    return m_cellWidth;
}

int Grid::GetCellHeight() const
{
    return m_cellHeight;
}

bool Grid::CheckCollision(float x, float y, float w, float h, int* id) const
{
    int closestId = -1;
    float closestDistance = 5000.0f;

    for (int i = 0; i < m_width; i++)
    {
        for (int j = 0; j < m_height; j++)
        {
            int idx = GetIndexFromPosition(i, j);
            if (m_gridData[idx] != nullptr && m_gridData[idx]->type != 0)
            {
                float worldX, worldY;
                ToWorld(i, j, &worldX, &worldY);
                float fW = static_cast<float>(m_cellWidth);
                float fH = static_cast<float>(m_cellHeight);
                if (Engine::CheckRects(x, y, w, h, worldX, worldY, fW, fH))
                {
                    float distance = Engine::DistanceBetweenRectangles(x, y, w, h, worldX, worldY, fW, fH);
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

int Grid::GetIndexFromPosition(int x, int y) const
{
    return x + m_width * y;
}

void Grid::ToWorld(const int localX, const int localY, float* worldX, float* worldY) const
{
    *worldX = static_cast<float>(localX * m_cellWidth) + m_offsetX;
    *worldY = static_cast<float>(localY * m_cellHeight) + m_offsetY;
}

void Grid::ToLocal(const float worldX, const float worldY, int* localX, int* localY) const
{
    *localX = static_cast<int>((worldX - m_offsetX) / m_cellWidth);
    *localY = static_cast<int>((worldY - m_offsetY) / m_cellHeight);
}

void Grid::GetWorldPositionFromIndex(int index, int* outX, int* outY) const
{
    int localX, localY;
    GetLocalPosition(index, m_width, &localX, &localY);
    *outX = localX * m_cellWidth + m_offsetX;
    *outY = localY * m_cellHeight + m_offsetY;
}

bool Grid::Hit(int idx)
{
    Tile* tile = m_gridData[idx];
    tile->Hit();

    if (tile->type == 0)
    {
        return false;
    }

    if (tile->type == 10)
    {
#if CAN_DESTROY_GOLD
        tile->type = 0;
        tile->hit = 0;
        return true;
#else
        return false;
#endif
    }

    if (tile->hit <= 0)
    {
        tile->type = 0;
        SaveGame::score += tile->score;
    }

    return tile->hit <= 0;
}

bool Grid::Cleared()
{
    for (int i = 0; i < m_width; i++)
    {
        for (int j = 0; j < m_height; j++)
        {
            int idx = GetIndexFromPosition(i, j);
            if (m_gridData[idx] != nullptr && m_gridData[idx]->type != 0 && m_gridData[idx]->type != 10)
            {
                return false;
            }
        }
    }

    return true;
}

bool Grid::LastLevel()
{
    return SaveGame::round > 35/*Level::data.size() - 1*/;
}

void StaticTile::Render(float x, float y, float w, float h)
{
    if (type == 0) return;

#if SHOW_DEBUG_GRID_BLOCK
    Engine::FillRect(x, y, w, h, NColor::LightGreen);
    Engine::DrawRect(x, y, w, h, NColor::Black);
#else
    Rect<float> dst = { x, y, w, h };
    Engine::DrawTexture(m_texture, source, dst, 0.0, false, false, NColor::White);
#endif
}

void AnimatedTile::Init(const std::string& filename, int frameInRows, int frameWidth, int frameHeight)
{
    anim.Init(filename, frameInRows, frameWidth, frameHeight);
    anim.AddClip("idle", 0, 1, 0.0f);
    anim.AddClip("flash", 0, 6, 0.03f);
    anim.Play("idle", false);
}

void AnimatedTile::Update(float dt)
{
    anim.Update(dt);
}

void AnimatedTile::Render(float x, float y, float w, float h)
{
    if (type == 0) return;

#if SHOW_DEBUG_GRID_BLOCK
    Engine::FillRect(x, y, w, h, NColor::LightRed);
    Engine::DrawRect(x, y, w, h, NColor::Black);
#else
    Rect<float> dst = { x, y, w, h };
    anim.Render(dst);
#endif
}

void AnimatedTile::Hit()
{
    Tile::Hit();
    if (hit > 0)
    {
        anim.Play("flash", false);
    }
}

void Tile::Hit()
{
    if (type == 0 || type == 10) return;
    hit--;
}
