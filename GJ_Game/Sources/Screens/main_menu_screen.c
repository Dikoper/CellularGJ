/**********************************************************************************************
*
*   Title Screen Functions Definitions (Init, Update, Draw, Unload)
*
**********************************************************************************************/
#include "raylib.h"
#include "../Headers/screens.h"
#define RAYMATH_IMPLEMENTATION
#include "raymath.h"

void InitTitleScreen(void);
void UpdateTitleScreen(void);
void DrawTitleScreen(void);
void UnloadTitleScreen(void);
int FinishTitleScreen(void);

extern GameScreen main_menu_screen =
{
    TITLE,
    NONE,
    InitTitleScreen,
    UpdateTitleScreen,
    DrawTitleScreen,
    UnloadTitleScreen,
    FinishTitleScreen
};

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int finishScreen = 0;

//----------------------------------------------------------------------------------
// Title Screen Functions Definition
//----------------------------------------------------------------------------------

// Title Screen Initialization logic
void InitTitleScreen(void)
{
    // TODO: Initialize TITLE screen variables here!
    finishScreen = 0;
}

// Title Screen Update logic
void UpdateTitleScreen(void)
{
    // TODO: Update TITLE screen variables here!

    // Press enter or tap to change to GAMEPLAY screen
    if (IsKeyPressed(KEY_ENTER))
    {
        finishScreen = 2;   // GAMEPLAY
    }
}

// Title Screen Draw logic
void DrawTitleScreen(void)
{
    // TODO: Draw TITLE screen here!
    DrawTextEx(font, GAME_TITLE, Vector2Add((Vector2) { -200, -200 }, SCREEN_CENTRE) , font.baseSize * 12, 3, GRAY);
    DrawTextEx(font, "PRESS ENTER to proceed", Vector2Add((Vector2) { -200, 0 }, SCREEN_CENTRE), font.baseSize * 3, 1, RAYWHITE);
}

// Title Screen Unload logic
void UnloadTitleScreen(void)
{
    // TODO: Unload TITLE screen variables here!
}

// Title Screen should finish?
int FinishTitleScreen(void)
{
    main_menu_screen.state = FINISHING;
    return finishScreen;
}