#include "game_screen.h"

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int finishScreen;
Vector2 textPos;
Vector2 offset;

Object_t point;

//----------------------------------------------------------------------------------
// Title Screen Functions Definition
//----------------------------------------------------------------------------------

// Title Screen Initialization logic
void InitGameScreen(void)
{
    // TODO: Initialize TITLE screen variables here!
    point.name = "text";
    point.ID = GetStringHash(point.name, length(point.name));
    point.transform.position = SCREEN_CENTRE;
    point.is_active = true;

    finishScreen = 0;
}

// Title Screen Update logic
void UpdateGameScreen(void)
{
    // TODO: Update Game screen variables here!

    offset = (Vector2){ cosf(GetTime()) * 20, -sinf(GetTime()) *20 };

    // Press enter or tap to change to GAMEPLAY screen
    if (IsKeyPressed(KEY_ENTER))
        finishScreen = 1;   //menu

    if (IsKeyPressed(KEY_SPACE))
        point.is_active = !point.is_active;

    if (point.is_active)
    {
        if (IsKeyDown(KEY_LEFT))
            --point.transform.position.x;
        if (IsKeyDown(KEY_RIGHT))
            ++point.transform.position.x;
        if (IsKeyDown(KEY_UP))
            --point.transform.position.y;
        if (IsKeyDown(KEY_DOWN))
            ++point.transform.position.y;
    }
}

// Game Screen Draw logic
void DrawGameScreen(void)
{
    // TODO: Draw Game screen here!
    DrawText("Use arrows to move the text", SCREEN_CENTRE.x, 10, 10, RAYWHITE);
    if (point.is_active)
    {
        DrawTextEx(font, FormatText("%c", 64), Vector2Add(point.transform.position, offset), font.baseSize * 8, 3, GRAY);
        DrawCircle(point.transform.position.x, point.transform.position.y, 10, RAYWHITE);
    }
}

// Game Screen Unload logic
void UnloadGameScreen(void)
{
    // TODO: Unload Game screen variables here!
}

// Game Screen should finish?
int FinishGameScreen(void)
{
    return finishScreen;
}