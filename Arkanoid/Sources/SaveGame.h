#pragma once
#include "File.h"

class SaveGame
{
public:
    static int highScore;
    static int round;
    static int life;
    static int score;

    static void CheckHighScore(int score);
    static void Save();
    static void Load();
    static void NextRound();

private:
    static CFile m_saveGameFile;
};