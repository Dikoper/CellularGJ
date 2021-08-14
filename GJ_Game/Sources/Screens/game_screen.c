/**********************************************************************************************
*
*   Gameplay Screen Functions Definitions (Init, Update, Draw, Unload)
*
**********************************************************************************************/

#include "raylib.h"
#include "../Headers/screens.h"
#include "../Headers/extensions.h"
#define RAYMATH_IMPLEMENTATION
#include "raymath.h"
#include <string.h>


//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;

#define m 8

#define CS 32/m     // cell size    
#define GW 48*m     // grid width
#define GH 32*m     // grid height

static Vector2 screenCentre;

static bool paused = false;
// TODO: rewrite buffer stuff
static int prev_buf = 0;    // to alternate between cell buffers //TODO: rewrite this shit
static int curr_buf = 1;

static int pad;    // add a padding space around cells (just to look nicer)
static int pad2;

static int cells[2][GH][GW];

static Camera2D camera;
static Vector2 tx_v;
static Texture2D tx;
static Rectangle rc;
static bool running;

RenderTexture2D rt2D;
RenderTexture2D guiRT;

RuleAttrib stay_rule, birth_rule; // TODO: upgrade rules stuff

static float renderScale = 1.0f;

///
/// make buffer for both human and virus cells(prev, current)
/// cell type (char) 0,1,2,3...
///

//----------------------------------------------------------------------------------
// Gameplay Functions Definition
//----------------------------------------------------------------------------------

void StayBHV(int* cell, int prev, int neighbors, RuleAttrib ra)
{
    for (int i=0; i < NEIGHBORS_COUNT; ++i)
    {
        if (ra[i] == 1)
        {
            if (i == neighbors)
            {
                *cell = prev;
                break;
            }
        }
    }
}

void BirthBHV(int* cell, int prev, int neighbors, RuleAttrib ra)
{
    for (int i = 0; i < NEIGHBORS_COUNT; ++i)
    {
        if (ra[i] == 1)
        {
            if (i == neighbors)
            {
                *cell = 1;
                break;
            }
        }
    }
}

void RandomizeCells() {
    for (int buf = 0; buf < 2; buf++) {
        for (int j = 0; j < GH; j++) {
            for (int i = 0; i < GW; i++) {
                cells[buf][j][i] = GetRandomValue(0, 1);
            }
        }
    }
}

void InitCells() {
    pad = 1;        // top and left padding
    pad2 = pad * 2;   // bottom and right padding

    if (m > 4) {    // check if cells aren't too small, or else they become invisible with padding
        pad = 0;    // the condition should be adjusted if default padding values are changed
        pad2 = 0;
    }

    RandomizeCells();
}

void PassGeneration() {
    prev_buf = 1 - prev_buf;      // switch buffers
    curr_buf = 1 - curr_buf;

    for (int j = 0; j < GH; j++) {
        for (int i = 0; i < GW; i++) {

            // wrap around
            int j_ = j - 1 >= 0 ? j - 1 : GH - 1;
            int _j = j + 1 < GH ? j + 1 : 0;
            int i_ = i - 1 >= 0 ? i - 1 : GW - 1;
            int _i = i + 1 < GW ? i + 1 : 0;

            // count alive neighbors
            int n = cells[prev_buf][j_][i_]
                + cells[prev_buf][j_][i]
                + cells[prev_buf][j_][_i]
                + cells[prev_buf][j][i_]
                + cells[prev_buf][j][_i]
                + cells[prev_buf][_j][i_]
                + cells[prev_buf][_j][i]
                + cells[prev_buf][_j][_i];

            // set cell according to rules
            //cells[curr_buf][j][i] = (int)n == 3 || (cells[prev_buf][j][i] && n == 2);

            cells[curr_buf][j][i] = 0; //default stable is death
            int* curr_cell = &cells[curr_buf][j][i];
            int* prev_cell = &cells[prev_buf][j][i];

            Vector4 rule = { 2,3,3,3 };
            //stay_rule = {0,0,0,0,0,0,0,0,0};
            //stay_rule = (RuleAttrib){ 0,0,1,1 };
            //birth_rule = (RuleAttrib){ 0,0,0,1 };

            StayBHV(curr_cell, *prev_cell, n, stay_rule);

            BirthBHV(curr_cell, *prev_cell, n, birth_rule);
        }
    }
}

void SpawnFigure(Vector2 pos)
{
    int figure[16] = 
    {
        0,1,0,0,
        0,0,1,0,
        1,1,1,0,
        0,0,0,0
    };

    for (size_t i = 0, n=0; i < 4; i++)
    {
        for (size_t j = 0; j < 4; j++)
        {
            cells[0][(int)pos.x + i][(int)pos.y + j] = figure[n];
            cells[1][(int)pos.x + i][(int)pos.y + j] = figure[n];
            ++n;
        }
    }

}

void WipeGrid(int wiper)
{
    prev_buf = 1 - prev_buf;      // switch buffers
    curr_buf = 1 - curr_buf;

    for (int j = 0; j < GH; j++) {
        for (int i = 0; i < GW; i++) {
            // set cell according to rules
            cells[curr_buf][j][i] = wiper;
        }
    }
}

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//----------------------------------------------------------------------------------

// Gameplay Screen Initialization logic
void InitGameplayScreen(void)
{
    // TODO: Initialize GAMEPLAY screen variables here!
    framesCounter = 0;
    finishScreen = 0;

    int screenWidth = GW * CS;
    int screenHeight = GH * CS;

    // set window size according to grid and cell sizes
    screenCentre = (Vector2){ screenWidth / 2.0f, screenHeight / 2.0f };
    SetWindowSize(screenWidth, screenHeight);
    // set virtual camera
    camera.target = screenCentre;
    camera.offset = screenCentre;
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    SetTargetFPS(144);
    running = true;

    GuiLoadStyle("../Vendors/styles/cyber/cyber.rgs");
    InitCells();

    rt2D = LoadRenderTexture(screenWidth * renderScale, screenHeight * renderScale);
    tx = LoadTexture("Resources/Images/cell.png");
    rc = (Rectangle){ 100,200,100,201 };
}

// Gameplay Screen Update logic
void UpdateGameplayScreen(void)
{
    // TODO: Update GAMEPLAY screen variables here!

    // Press enter or tap to change to ENDING screen
    if (IsKeyPressed(KEY_ESCAPE))
    {
        finishScreen = 1;
        //PlaySound(fxCoin);
    }

    if (IsKeyPressed(KEY_SPACE))                    paused = !paused;
    if (IsKeyPressed(KEY_ENTER))                    RandomizeCells();

    if (IsKeyDown(KEY_UP))      camera.target = (Vector2){ camera.target.x, camera.target.y - 10 };
    if (IsKeyDown(KEY_DOWN))    camera.target = (Vector2){ camera.target.x, camera.target.y + 10 };
    if (IsKeyDown(KEY_LEFT))    camera.target = (Vector2){ camera.target.x - 10, camera.target.y };
    if (IsKeyDown(KEY_RIGHT))   camera.target = (Vector2){ camera.target.x + 10, camera.target.y };

    // Camera zoom controls
    if ((GetMouseWheelMove() > 0.1f))
    {
        camera.zoom += ((float)GetMouseWheelMove() * 0.05f);
    }
    else if ((GetMouseWheelMove() < -0.1f))
    {
        camera.zoom += ((float)GetMouseWheelMove() * 0.05f);
    }

    if (camera.zoom > 10.0f) camera.zoom = 10.0f;
    else if (camera.zoom < 0.1f) camera.zoom = 0.1f;

    if (paused) {
        if (IsKeyPressed(KEY_G))
            PassGeneration();
    }
    else {
        PassGeneration();
    }

}

// Maybe use some culling(texture is too big)
void RenderGridToTexture(RenderTexture2D rt) 
{
    BeginTextureMode(rt);
        ClearBackground(BLACK);
        for (int j = 0; j < GH; j++) {
            for (int i = 0; i < GW; i++) {
                int x = renderScale * i * CS + pad;
                int y = renderScale * j * CS + pad;
                int w = CS - pad2;
                int h = CS - pad2;

                tx_v = (Vector2){ x, y };

                if (cells[curr_buf][j][i])
                    DrawTextureEx(tx, tx_v, 0, renderScale / m, WHITE);
            }
        }
    EndTextureMode();
}

void RenderGUI() 
{
    //ClearBackground(BLACK);
    //GuiPanel((Rectangle) { 0, 0, 300, screenHeight });
    //if (GuiButton((Rectangle) { 10, 100, 100, 120 }, "#152#"))
        // SpawnFigure(0, 0);
    
    paused = GuiToggle((Rectangle) { 30, 30, 120, 60 }, "#132#", paused);
    if (GuiButton((Rectangle) { 30, 90, 120, 60 }, "#134#"))
        PassGeneration();

    if (GuiButton((Rectangle) { 30, 150, 120, 60 }, "#79#"))
        WipeGrid(0);

    if (GuiButton((Rectangle) { 30, 210, 120, 60 }, "#152#"))
        SpawnFigure((Vector2) {GW/2,GH/2});

    char s_str[10] = { 0 }, b_str[10] = {0};
    
    for (int i = 0; i < NEIGHBORS_COUNT; ++i) 
    {
        // TODO: rewrite for values more that 1 (0,1,2,3...)
        if (stay_rule[i] == 1) 
        {
            strcat_s(s_str, sizeof(s_str), TextFormat("%d", i));
        }
        if (birth_rule[i] == 1)
        {
            strcat_s(b_str, sizeof(b_str), TextFormat("%d", i));
        }
    }

    DrawText(s_str, 250, 10, 28, RED);
    DrawText(b_str, 250, 30, 28, BLUE);

    DrawFPS(10, 10);
}

// Gameplay Screen Draw logic
void DrawGameplayScreen(void)
{
    // TODO: Draw GAMEPLAY screen here!

    // Screen draw
    RenderGridToTexture(rt2D);

    BeginMode2D(camera);
        ClearBackground(GRAY);
        DrawTextureEx(rt2D.texture, (Vector2) {0,0}, 0, 1, WHITE);
    EndMode2D();
    
    RenderGUI();
}

// Gameplay Screen Unload logic
void UnloadGameplayScreen(void)
{
    // TODO: Unload GAMEPLAY screen variables here!
    UnloadRenderTexture(rt2D);
    UnloadTexture(tx);
}

// Gameplay Screen should finish?
int FinishGameplayScreen(void)
{
    return finishScreen;
}