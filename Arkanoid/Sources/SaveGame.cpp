#include "SaveGame.h"
#include "Config.h"

int SaveGame::highScore = 50000;
int SaveGame::life = STARTING_LIFE;
int SaveGame::round = START_AT_ROUND;
int SaveGame::score = 0;

BufferedFile SaveGame::m_saveGameFile;

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
    m_saveGameFile.CreateBuffer(sizeof(int) * 1);
    m_saveGameFile.Seek(0);
    m_saveGameFile.WriteInt32(highScore);
    m_saveGameFile.WriteToDisk("arkanoid.sav");
    m_saveGameFile.CloseBuffer();
}

void SaveGame::Load()
{
    life = STARTING_LIFE;
    round = START_AT_ROUND;
    score = 0;

    if (BufferedFile::PeekVersion("arkanoid.sav") > 0)
    {
        m_saveGameFile.LoadFromDisk("arkanoid.sav");
        m_saveGameFile.ReadInt32(&highScore);
        m_saveGameFile.CloseBuffer();
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
