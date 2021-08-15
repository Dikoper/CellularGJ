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

#define m 4

#define CS 32/m     // cell size
#define GW 48*m     // grid width
#define GH 32*m     // grid height

#define TICKRATE 1000/5/1000 // update grid 5 times in a second

double lastTick;
size_t ticksCount;

static Vector2 screenCentre;

static bool paused = false;
// TODO: rewrite buffer stuff
static int prev_buf = 0;    // to alternate between cell buffers //TODO: rewrite this shit
static int curr_buf = 1;

static int pad;    // add a padding space around cells (just to look nicer) ?
static int pad2;

// TODO: unite in one buffer to get rid of memcopy
static int nextState[GH * GW];
static int currentState[GH * GW];

static Camera2D camera;
static Vector2 tx_v;
static Texture2D tx;
static Rectangle rc;
static bool running;

static float renderScale = 1.0f;
RenderTexture2D rt2D;
RenderTexture2D guiRT;

RuleAttrib stay_rule, birth_rule; // TODO: upgrade rules stuff

static int nb[NEIGHBORS_COUNT] = {0};

//----------------------------------------------------------------------------------
// Gameplay Functions Definition
//----------------------------------------------------------------------------------

void RandomizeCells() 
{
    for (int i = 0; i < GH; i++) {
        for (int j = 0; j < GW; j++) {
            int rnd = GetRandomValue(0, 2);
            int indx = i * GW + j;
            nextState[indx] = rnd;
            currentState[indx] = rnd;
        }
    }
}

void InitCells() {
    // Am i need this?
    pad = 1;        // top and left padding
    pad2 = pad * 2;   // bottom and right padding

    if (m > 4) {    // check if cells aren't too small, or else they become invisible with padding
        pad = 0;    // the condition should be adjusted if default padding values are changed
        pad2 = 0;
    }

    RandomizeCells();
}

int WarpCoord(int x, int y, Vector2 size)
{
    if (y >= size.y) 
        y = 0;
    if (x >= size.x)
        x = 0;
    if (y < 0) 
        y = size.y - 1;
    if (x < 0) 
        x = size.x - 1;

    int idx = y * GW + x;

    return idx;
}

int* GetNeighbors(int* buffer, Vector2 pos, int *out, Vector2 warp)
{
    /// <summary>
    /// OutputIndexesLayout
    /// 1|2|3
    /// 4|0|5
    /// 6|7|8
    /// </summary>

    int neighbors[NEIGHBORS_COUNT] = {0};

    int y = (int)pos.x;
    int x = (int)pos.y;

    //while (i < 2)
    //{
    //    while (j < 2)
    //    {
    //        _i = x + i;
    //        if ((x + i >= warp.x))
    //            _i = 0;
    //        if ((x + i < 0))
    //            _i = warp.x - 1;
    //            
    //        //
    //        _j = y + j;
    //        if ((y + j >= warp.y))
    //            _j = 0;

    //        if ((y + j < 0))
    //            _j = warp.y - 1;
    //        
    //        int b = buffer[_i * (int)warp.y + _j];
    //        if (i == 0 && j == 0) {
    //            out[0] = b;
    //            neighbors[0] = b;
    //        }
    //        else
    //        {
    //            out[n] = b;
    //            neighbors[n] = b;
    //            ++n;
    //        }
    //        ++j;
    //    }
    //    j = -1;
    //    ++i;
    //}

    //warp

    out[0] = buffer[WarpCoord(x, y, warp)];
    out[1] = buffer[WarpCoord(x -1, y-1, warp)];
    out[2] = buffer[WarpCoord(x, y-1, warp)];
    out[3] = buffer[WarpCoord(x+1, y-1, warp)];
    out[4] = buffer[WarpCoord(x-1, y, warp)];
    out[5] = buffer[WarpCoord(x+1, y, warp)];
    out[6] = buffer[WarpCoord(x-1, y+1, warp)];
    out[7] = buffer[WarpCoord(x, y+1, warp)];
    out[8] = buffer[WarpCoord(x+1, y + 1, warp)];

    return out;
}

void PassGeneration() {
    prev_buf = 1 - prev_buf;      // switch buffers
    curr_buf = 1 - curr_buf;

    for (int j = 0; j < GH; j++) 
    {
        for (int i = 0; i < GW; i++) 
        {
            int indx = j * GW + i;

            GetNeighbors( currentState, (Vector2) { j, i }, &nb, (Vector2) { GW, GH });
            
            //int n_count = 0;
            //for (size_t i = 1; i < NEIGHBORS_COUNT; i++) //zero index is for cell itself
            //{
            //    if (nb[i] >= 1)
            //        n_count++;
            //}
            //TraceLog(LOG_INFO, TextFormat("cell - %d,%d \t n - %d", i,j,n_count ));

            nextState[indx] = 0;//default stable is death

            StayBHV(&nextState[indx], currentState[indx], &nb, stay_rule);
            BirthBHV(&nextState[indx], currentState[indx], &nb, birth_rule);
        }
    }
    // TODO: delete mem copy
    memcpy(currentState, nextState, sizeof(currentState));
}

void SpawnFigure(int x, int y)
{ // TODO: Rework for spawn screen setting feature
    int SIZE = 9;
    int figure[9] = 
    {
        /*0,1,0,0,
        0,0,1,0,
        1,1,1,0,
        0,0,0,0*/
        0,0,0,
        1,1,1,
        0,0,0,
    };

    for (size_t j = 0, n=0; j < sqrt(SIZE); j++)
    {
        for (size_t i = 0; i < sqrt(SIZE); i++)
        {
            size_t indx = (y + j) * GW + x + i;
            nextState[indx] = figure[n];
            currentState[indx] = figure[n];
            ++n;
        }
    }

}

void WipeGrid(int wiper)
{
    prev_buf = 1 - prev_buf;      // switch buffers
    curr_buf = 1 - curr_buf;

    for (int i = 0; i < GW; i++) {
        for (int j = 0; j < GH; j++) {
            nextState[i * GH + j] = wiper;
            currentState[i * GH + j] = wiper;
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

    int screenWidth = (GW * CS) >= 1280 ? GW * CS : 1280;
    int screenHeight = (GH * CS) >= 720 ? GH * CS : 720;

    GuiLoadStyle("../Vendors/styles/cyber/cyber.rgs");
    rt2D = LoadRenderTexture(screenWidth * renderScale, screenHeight * renderScale);
    tx = LoadTexture("Resources/Images/cell.png");
    rc = (Rectangle){ 100,200,100,201 };

    // TODO : remember to free mem
    if (nextState == NULL || currentState == NULL)
        TraceLog(LOG_ERROR, "Cannot allocate memory for buffers");

    /*int testBuff[9] =
    {
        1,0,3,
        2,0,2,
        0,1,0
    };
    int n[9] = { 1,2,3,4,5,6,7,8,9 };
    GetNeighbors(testBuff, (Vector2) { 2, 2 }, &n, (Vector2) { 3, 3 });
    TraceLog(LOG_INFO, TextFormat("test n - %d", n));*/

    // set window size according to grid and cell sizes
    screenCentre = (Vector2){ screenWidth / 2.0f, screenHeight / 2.0f };
    SetWindowSize(screenWidth, screenHeight);

    SetTargetFPS(60);

    InitCells();
    // set virtual camera
    camera.target = screenCentre;
    camera.offset = screenCentre;
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    running = true;
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
    // TODO: redo mouse cam controls
    if (IsKeyDown(KEY_UP))      camera.target = (Vector2){ camera.target.x, camera.target.y - 10 };
    if (IsKeyDown(KEY_DOWN))    camera.target = (Vector2){ camera.target.x, camera.target.y + 10 };
    if (IsKeyDown(KEY_LEFT))    camera.target = (Vector2){ camera.target.x - 10, camera.target.y };
    if (IsKeyDown(KEY_RIGHT))   camera.target = (Vector2){ camera.target.x + 10, camera.target.y };
    
    if (IsKeyPressed(KEY_C))
        CellsCount(currentState, (sizeof(currentState) / sizeof(currentState[0])));
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

    if (paused) 
    {
        if (IsKeyPressed(KEY_G))
            PassGeneration();
    }
    else
    {
        if (GetTime() - lastTick > TICKRATE)
        {
            PassGeneration();
            lastTick = GetTime();
            ticksCount++;
        }   
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

                if(nextState[j * GW + i] == PLAYER_CELL)
                    DrawTextureEx(tx, tx_v, 0, renderScale / m, GREEN);
                else
                    if(nextState[j * GW + i] == ENEMY_CELL)
                        DrawTextureEx(tx, tx_v, 0, renderScale / m, RED);
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
        SpawnFigure(GW/2, GH/2);

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

    DrawText(TextFormat("Epoch - %d", ticksCount), 500, 10, 28, RAYWHITE);

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

    MemFree(*nextState);
    MemFree(*currentState);
}

// Gameplay Screen should finish?
int FinishGameplayScreen(void)
{
    return finishScreen;
}