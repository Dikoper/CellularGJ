/**********************************************************************************************
*
*   Game Screen Functions Definitions (Init, Update, Draw, Unload)
*
**********************************************************************************************/
#pragma once

#include "raylib.h"
#include "raymath.h"
#include "../Headers/objects.h"
#include "../Headers/screens.h"


void InitGameScreen(void);
void UpdateGameScreen(void);
void DrawGameScreen(void);
void UnloadGameScreen(void);
int FinishGameScreen(void);

extern GameScreen game_screen =
{
    GAMEPLAY,
    NONE,
    InitGameScreen,
    UpdateGameScreen,
    DrawGameScreen,
    UnloadGameScreen,
    FinishGameScreen
};