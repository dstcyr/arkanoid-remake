#include "SaveGame.h"
#include "Config.h"

int SaveGame::highScore = 50000;
int SaveGame::life = STARTING_LIFE;
int SaveGame::round = START_AT_ROUND;
int SaveGame::score = 0;

CFile SaveGame::m_saveGameFile;

void SaveGame::CheckHighScore()
{
    if (score > highScore)
    {
        highScore = score;
    }

    Save();
}

void SaveGame::Save()
{
    m_saveGameFile.Seek(0);
    m_saveGameFile.WriteInt(highScore);
    m_saveGameFile.Save("arkanoid.sav");
    m_saveGameFile.Release();
}

void SaveGame::Load()
{
    life = STARTING_LIFE;
    round = START_AT_ROUND;
    score = 0;

    if (CFile::Exists("arkanoid.sav"))
    {
        m_saveGameFile.Load("arkanoid.sav");
        highScore = m_saveGameFile.ReadInt();
        m_saveGameFile.Release();
    }
    else
    {
        highScore = 50000;
    }
}

void SaveGame::NextRound()
{
    round++;
}
