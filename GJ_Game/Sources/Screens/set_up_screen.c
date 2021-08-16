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
            Rectangle c_rc = { x + i * cellSize * 2, y + j * cellSize *1.2, pd*2, pd*1.2 };
            initFigure[idx] = GuiCheckBox(c_rc, "", initFigure[idx]);
        }
    }
}

// Setup Screen Draw logic
void DrawSetupScreen(void)
{
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), BLACK);

    DrawTextEx(font, "STILL", (Vector2) { 620, 50 }, font.baseSize * 2, 3, GRAY);
    //stay
    for (int i=0 ; i < 3; ++i) 
    {
        for (size_t j = 0; j < 3; j++)
        {
            GuiSpinner((Rectangle) { 450 + 20 + i * 120, 100 + j * 50, 100, 25 }, TextFormat("s%d", j*3+i), &pStillRule[j * 3 + i], 0, 2, false);
        }
        
    }
    DrawTextEx(font, "BIRTH", (Vector2) { 1040, 50 }, font.baseSize * 2, 3, GRAY);
    //birth
    for (int i = 0; i < 3; ++i)
    {
        for (size_t j = 0; j < 3; j++)
        {
            GuiSpinner((Rectangle) { 875 + 20 + i * 120, 100 + j * 50, 100, 25 }, TextFormat("b%d", j*3 +i), &pBirthRule[j * 3 + i], 0, 2, false);
        }

    }

    DrawGrid((Rectangle) { 55, 105, GRID_X, GRID_Y}, 32);
    GuiGroupBox((Rectangle) { 50, 100, GRID_X * 32 * 2 + 10, GRID_Y * 32 * 1.2 +10}, "Figure");

    const char* controlsText =
        "Choose your tactics of BIRTH and STILL to unleash the alien's capabilities"
        "\nTry to build different life forms in figure window"
        "\n\nGame controls :"
        "\n Use WASD keys to move"
        "\n Use mouse wheel to zoom in(out)"
        "\n Press pause button to pause game"
        "\n Press Play button to speed up"
        "\n Press escape key to get back";

    DrawText(controlsText, 50, 425, 20, DARKGRAY);

    // TODO: Draw OPTIONS screen here!
    if (GuiButton((Rectangle) { GetScreenWidth() - 350, GetScreenHeight() - 150, 300, 125 }, "Start experiment.."))
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