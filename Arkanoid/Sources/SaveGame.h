#pragma once
#include "save/BufferedFile.h"

class SaveGame
{
public:
    static int highScore;
    static int round;
    static int life;
    static int score;

    static void CheckHighScore();
    static void Save();
    static void Load();
    static void NextRound();

private:
    static BufferedFile m_saveGameFile;
};