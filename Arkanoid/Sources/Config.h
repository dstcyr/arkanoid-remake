#pragma once

#define SAFE_DELETE(ptr) if(ptr) { delete ptr; ptr = nullptr; }

const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 962;
const float SIDE_WIDTH = 8;
const float SIDE_ZONE_WIDTH = 32;
const float SHARP_ANGLE = 30.0f;
const float STEEP_ANGLE = 45.0f;
const float SHALLOW_ANGLE = 75.0f;
const int BLOCK_WIDTH = 64;
const int BLOCK_HEIGHT = 32;
const int GRID_WIDTH = 11;
const int GRID_HEIGHT = 16;
const int PLAYGROUND_OFFSET_X = 40;
const int GRID_OFFSET_X = 33;
const int GRID_OFFSET_Y = GRID_OFFSET_X + 70;
const float LEFT_WALL_X = 73.0f;
const float RIGHT_WALL_X = LEFT_WALL_X + BLOCK_WIDTH * GRID_WIDTH;
const float TOP_WALL_Y = 33.0f;
const float BOTTOM_WALL_Y = 960.0f;
const int CHANCE_TO_SPAWN_CAPSULE = 20;
const float CAPSULE_FALLING_SPEED = 100.0f;
const float LASER_WIDTH = 14;
const float LASER_HEIGHT = 14;
const float LASER_SPEED = 800.0f;
const float BALL_START_X = 400.0f;
const float BALL_START_Y = 650.0f;
const float SHIP_START_X = 400.0f;
const float SHIP_START_Y = 860.0f;
const float GAME_SPEED = 500.0f;
const float SLOWDOWN_ITR = 0.5f;
const float SLOWDOWN_DELAY = 15.0f;
const int LAST_LEVEL = 35;
const double SFX_DELAY = 100.0;

const int EMPTY_TILE = 0;
const int HARD_TILE = 9;
const int GOLD_TILE = 10;

#define CHECK_MEMORY_LEAKS 0
#define CREATE_DEBUG_CONSOLE 1
#define PAUSE_CONSOLE_WHEN_CLOSING 0
#define START_IN_GAME 0
#define START_IN_INTRO 0
#define SHOW_BALL_DEBUG 0
#define SHOW_DEBUG_CAPSULE 0
#define SHOW_PADDLE_DEBUG 0
#define SHOW_PADDLE_DEBUG_COLLISION 0
#define SHOW_PADDLE_DEBUG_ZONES 0
#define SHOW_DEBUG_GRID 0
#define SHOW_DEBUG_GAME_BOUNDARY 0
#define USE_SOUNDFX 0
#define SHOW_DEBUG_LASER 0
#define TOGGLE_PADDLE_DEBUG 0
#define ALWAYS_SPAWN_EXPAND 0
#define ALWAYS_SPAWN_SLOW 0
#define ALWAYS_SPAWN_CATCH 0
#define ALWAYS_SPAWN_DISRUPT 0
#define ALWAYS_SPAWN_LASER 0
#define ALWAYS_SPAWN_WARP 0
#define START_AT_ROUND 1
#define STARTING_LIFE 2
#define DRAW_DEBRIS_DEBUG 0
#define DEBRIS_IMMUNE_FROM_BALL 0
#define SKIP_PLAYER_READY 0
#define SHOW_MOCK_GAMEPLAY 0
#define MENU_MOCKUP 0


#define GOD_MODE 0





