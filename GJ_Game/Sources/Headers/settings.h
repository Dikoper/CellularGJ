#pragma once
#define DEBUG
#define GAME_TITLE "TestGame"

typedef struct 
{   
    const int screenWidth;
    const int screenHeight;
    const int framerate;
}GameSettings_t;

static GameSettings_t default_settings =
{
    .screenWidth = 1280,
    .screenHeight = 720,
    .framerate = 60
};
