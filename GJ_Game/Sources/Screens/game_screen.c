/**********************************************************************************************
*
*   Gameplay Screen Functions Definitions (Init, Update, Draw, Unload)
*
**********************************************************************************************/

#include "raylib.h"
#include "../Headers/screens.h"
#define RAYMATH_IMPLEMENTATION
#include "raymath.h"

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;

#define m 8

#define CS 32/m     // cell size    
#define GW 48*m     // grid width
#define GH 32*m     // grid height

static int screenWidth = GW * CS;
static int screenHeight = GH * CS;
static Vector2 screenCentre;

static bool paused = false;

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

static float renderScale = 1.0f;

//static struct Rule {
//    int[] n = { 8 };//read about arrays!
//};

//----------------------------------------------------------------------------------
// Gameplay Functions Definition
//----------------------------------------------------------------------------------

void StayBHV(int* cell, int prev, int neighbors, int attrib_1, int attrib_2)
{
	if ((neighbors >= attrib_1) && (neighbors <= attrib_2)) // stay
		*cell = prev;
}

void BirthBHV(int* cell, int prev, int neighbors, int attrib_1, int attrib_2)
{
	if ((neighbors >= attrib_1) && (neighbors <= attrib_2)) // birth ( is this needed? ->!cells[prev_buf][j][i] &&)
		*cell = 1;
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

            StayBHV(curr_cell, *prev_cell, n, rule.x, rule.y);

            BirthBHV(curr_cell, *prev_cell, n, rule.z, rule.w);
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
    GuiPanel((Rectangle) { 0, 0, 300, screenHeight });
    //if (GuiButton((Rectangle) { 10, 100, 100, 120 }, "#152#"))
        // SpawnFigure(0, 0);
    
    paused = GuiToggle((Rectangle) { 30, 30, 120, 60 }, "#132#", paused);
    if (GuiButton((Rectangle) { 30, 90, 120, 60 }, "#134#"))
        PassGeneration();

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