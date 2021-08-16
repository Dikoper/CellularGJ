/**********************************************************************************************
* 
*   Setup Screen Functions Definitions (Init, Update, Draw, Unload)
*
**********************************************************************************************/

#include "raylib.h"
#include "../Headers/screens.h"
#include "../Headers/extensions.h"

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;

Figure initFigure;

int val = 0;

RuleAttrib pStillRule, pBirthRule;

//static bool recs[GRID_X * GRID_Y] = { 0 };

//----------------------------------------------------------------------------------
// Setup Screen Functions Definition
//----------------------------------------------------------------------------------

// Setup Screen Initialization logic
void InitSetupScreen(void)
{
    // TODO: Initialize OPTIONS screen variables here!
    framesCounter = 0;
    finishScreen = 0;

    //default for testing
    pStillRule[2] = pStillRule[3] = 1;
    pBirthRule[3] = 1;
    
}

// Setup Screen Update logic
void UpdateSetupScreen(void)
{
    // TODO: Update OPTIONS screen variables here!
}
#include "raygui.h"

void DrawGrid(Rectangle rec,int cellSize)
{
    int x = rec.x;
    int y = rec.y;
    int pd = cellSize - 1;
    
    for (int i = 0; i < rec.width; i++)
    {
        for (int j = 0; j < rec.height; j++)
        {
            int idx = i * (int)rec.height + j;
            Rectangle c_rc = { x + i * cellSize, y + j * cellSize, pd, pd };
            initFigure[idx] = GuiCheckBox(c_rc, "", initFigure[idx]);
        }
    }
}

// Setup Screen Draw logic
void DrawSetupScreen(void)
{
    //stay
    for (int i=0 ; i < NEIGHBORS_COUNT; ++i) 
    {
        GuiSpinner((Rectangle) { 20 + i * 120, 0, 100, 25 }, TextFormat("s%d", i), &pStillRule[i], 0, 2, false);
    }
    //birth
    for (int i = 0; i < NEIGHBORS_COUNT; ++i)
    {
        GuiSpinner((Rectangle) { 20 + i * 120, 25, 100, 25 }, TextFormat("b%d", i), &pBirthRule[i], 0, 2, false);
    }

    DrawGrid((Rectangle) { 10, 100, GRID_X, GRID_Y}, 32);
    GuiGroupBox((Rectangle) { 10, 100, GRID_X * 32, GRID_Y * 32 }, "Figure");

    const char* controlsText =
        "Game controls :"
        "\n Use arrow keys to move camera"
        "\n Use mouse wheel to zoom in(out)"
        "\n Press pause button to pause game"
        "\n Press next button to pass one generation"
        "\n Press r key to wipe all cells"
        "\n Press figure key to spawn figure (glider)"
        "\n Press enter key to generate new cells";
        "\n Press escape key to get back to title screen";

    DrawText(controlsText, SCREEN_CENTRE.x - 300, GetScreenHeight()-200, 14, DARKGRAY);

    // TODO: Draw OPTIONS screen here!
    if (GuiButton((Rectangle) { SCREEN_CENTRE.x, SCREEN_CENTRE.y, 400, 250 }, "Process to game"))
    {
        finishScreen = 1;
    }
}

// Setup Screen Unload logic
void UnloadSetupScreen(void)
{
    // TODO: Unload OPTIONS screen variables here!
}

// Setup Screen should finish?
int FinishSetupScreen(void)
{
    return finishScreen;
}