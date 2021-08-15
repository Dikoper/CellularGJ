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

#define m 2

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

static int cells[2][GH][GW]; // move to 1D buffers

//cells buffers(1-player, 2-enemy)
//static int* nextState;
//static int* currCellsBuffer2;
//static int* currentState;
//static int* prevCellsBuffer2;

static int nextState[GH * GW];
static int currentState[GH * GW];
static int currCellsBuffer2[GH * GW];
static int prevCellsBuffer2[GH * GW];



static Camera2D camera;
static Vector2 tx_v;
static Texture2D tx;
static Rectangle rc;
static bool running;

static float renderScale = 1.0f;
RenderTexture2D rt2D;
RenderTexture2D guiRT;

RuleAttrib stay_rule, birth_rule, kill_rule; // TODO: upgrade rules stuff

static int nb[NEIGHBORS_COUNT] = {0};

///
/// make buffer for both human and virus cells(prev, current)
/// cell type (char) 0,1,2,3...
///

//----------------------------------------------------------------------------------
// Gameplay Functions Definition
//----------------------------------------------------------------------------------

void RandomizeCells() {
    // [i][j] index = [i * GH + j]  i*(max of j)+j

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
    pad = 1;        // top and left padding
    pad2 = pad * 2;   // bottom and right padding

    if (m > 4) {    // check if cells aren't too small, or else they become invisible with padding
        pad = 0;    // the condition should be adjusted if default padding values are changed
        pad2 = 0;
    }

    RandomizeCells();
}

size_t CoordsToIndex(int x, int y) // TODO: rewrite with macro
{
    return x * GH + y;
}

int WarpCoord(int x, int y, Vector2 warp)
{
    if (y >= warp.y) 
        y = 0;
    if (x >= warp.x) 
        x = 0;
    if (y < 0) 
        y = warp.y - 1;
    if (x < 0) 
        x = warp.x - 1;

    int idx = y * GH + x;

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

    //int _i, _j, n = 1;

    //int i = -1, j = -1;

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
    
    int bf;
    bf = buffer[8];

    bf = buffer[WarpCoord(x, y, warp)];
    out[0] = bf;

    out[1] = buffer[WarpCoord(x -1, y-1, warp)];
    out[2] = buffer[WarpCoord(x, y-1, warp)];
    out[3] = buffer[WarpCoord(x+1, y-1, warp)];
    out[4] = buffer[WarpCoord(x-1, y, warp)];
    out[5] = buffer[WarpCoord(x+1, y, warp)];
    out[6] = buffer[WarpCoord(x-1, y+1, warp)];
    out[7] = buffer[WarpCoord(x, y+1, warp)];
    out[8] = buffer[WarpCoord(x+1, y + 1, warp)];

    //for (size_t i = 0, n = 0; i < 3; i++)
    //{
    //    for (size_t j = 0; j < 3; j++)
    //    {
    // 
    //        int j_ = j - 1 >= 0 ? j - 1 : GH - 1;
    //        int _j = j + 1 < GH ? j + 1 : 0;
    //        int i_ = i - 1 >= 0 ? i - 1 : GW - 1;
    //        int _i = i + 1 < GW ? i + 1 : 0;

    //        // count alive neighbors
    //        
    //    }
    //}

    return out;
}

void PassGeneration() {
    prev_buf = 1 - prev_buf;      // switch buffers
    curr_buf = 1 - curr_buf;

    for (int j = 0; j < GH; j++) {
        for (int i = 0; i < GW; i++) {

            // wrap around
            //int j_ = j - 1 >= 0 ? j - 1 : GH - 1;
            //int _j = j + 1 < GH ? j + 1 : 0;
            //int i_ = i - 1 >= 0 ? i - 1 : GW - 1;
            //int _i = i + 1 < GW ? i + 1 : 0;

            //// count alive neighbors //rewrite neighbors logic
            //int n = cells[prev_buf][j_][i_]
            //    + cells[prev_buf][j_][i]
            //    + cells[prev_buf][j_][_i]
            //    + cells[prev_buf][j][i_]
            //    + cells[prev_buf][j][_i]
            //    + cells[prev_buf][_j][i_]
            //    + cells[prev_buf][_j][i]
            //    + cells[prev_buf][_j][_i];

            GetNeighbors( currentState, (Vector2) {j,i}, &nb, (Vector2) { GW, GH });
            
            //int n_count = 0;
            //for (size_t i = 1; i < NEIGHBORS_COUNT; i++) //zero index is for cell itself
            //{
            //    if (nb[i] >= 1)
            //        n_count++;
            //}
            //TraceLog(LOG_INFO, TextFormat("cell - %d,%d \t n - %d", i,j,n_count ));


            // set cell according to rules
            //cells[curr_buf][j][i] = (int)n == 3 || (cells[prev_buf][j][i] && n == 2);

            //cells[curr_buf][j][i] = 0; //default stable is death
            //int* curr_cell = &cells[curr_buf][j][i];
            //int* prev_cell = &cells[prev_buf][j][i];


            nextState[j*GW+i] = 0;//default stable is death

            StayBHV(&nextState[j * GW + i], currentState[j * GW + i], &nb, stay_rule);
            BirthBHV(&nextState[j * GW + i], currentState[j * GW + i], &nb, birth_rule);
        }
    }

    memcpy(currentState, nextState, sizeof(currentState));
}

void SpawnFigure(Vector2 pos)
{
    int SIZE = 9;
    int figure[9] = 
    {
        /*0,1,0,0,
        0,0,1,0,
        1,1,1,0,
        0,0,0,0*/
        1,0,0,
        1,0,0,
        1,0,0,
    };

    for (size_t j = 0, n=0; j < sqrt(SIZE); j++)
    {
        for (size_t i = 0; i < sqrt(SIZE); i++)
        {
            nextState[CoordsToIndex((int)pos.x + i, (int)pos.y + j)] = figure[n];
            currentState[CoordsToIndex((int)pos.x + i, (int)pos.y + j)] = figure[n];
            /*cells[0][(int)pos.x + i][(int)pos.y + j] = figure[n];
            cells[1][(int)pos.x + i][(int)pos.y + j] = figure[n];*/
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
            // set cell according to rules
            //cells[curr_buf][j][i] = wiper;
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
    // TODO : remember to free mem
    /*nextState = (int*)MemAlloc(GH * GW * sizeof(int));
    currCellsBuffer2 = (int*)MemAlloc(GH * GW * sizeof(int));
    currentState = (int*)MemAlloc(GH * GW * sizeof(int));
    prevCellsBuffer2 = (int*)MemAlloc(GH * GW * sizeof(int));*/

    /*int testBuff[9] =
    {
        1,0,3,
        2,0,2,
        0,1,0
    };
    int n[9] = { 1,2,3,4,5,6,7,8,9 };
    GetNeighbors(testBuff, (Vector2) { 2, 2 }, &n, (Vector2) { 3, 3 });
    TraceLog(LOG_INFO, TextFormat("test n - %d", n));*/


    size_t sz = sizeof(nextState) / sizeof(nextState[0]);

    if (nextState == NULL || currCellsBuffer2 == NULL || currentState == NULL|| currentState == NULL)
        return 1;

    // set window size according to grid and cell sizes
    screenCentre = (Vector2){ screenWidth / 2.0f, screenHeight / 2.0f };
    SetWindowSize(screenWidth, screenHeight);
    // set virtual camera
    camera.target = screenCentre;
    camera.offset = screenCentre;
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    SetTargetFPS(30);
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

                if(nextState[j*GW+i] == 1)
                    DrawTextureEx(tx, tx_v, 0, renderScale / m, GREEN);
                else
                    if(nextState[j * GW + i] == 2)
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
        SpawnFigure((Vector2) {0,0/*GW/2,GH/2*/});

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

    //WTF?
    //if (nextState != NULL) MemFree(nextState);
    //if (currCellsBuffer2 != NULL) MemFree(currCellsBuffer2);
   // if (currentState != NULL) MemFree(currentState);
   // if (prevCellsBuffer2 != NULL) MemFree(prevCellsBuffer2);
}

// Gameplay Screen should finish?
int FinishGameplayScreen(void)
{
    return finishScreen;
}