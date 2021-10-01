#pragma once

extern const char* GAME_TITLE = "CELL ENGINE";

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
