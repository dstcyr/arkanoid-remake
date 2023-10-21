#include "SaveGame.h"
#include "Config.h"

int SaveGame::highScore = 1000;
int SaveGame::life = 2;
int SaveGame::round = START_AT_ROUND;

CFile SaveGame::m_saveGameFile;

void SaveGame::CheckHighScore(int score)
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
    if (CFile::Exists("arkanoid.sav"))
    {
        m_saveGameFile.Load("arkanoid.sav");
        highScore = m_saveGameFile.ReadInt();
        m_saveGameFile.Release();
    }
    else
    {
        highScore = 1000;
    }
}

void SaveGame::NextRound()
{
    round++;
}
