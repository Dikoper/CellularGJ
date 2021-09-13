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

#define EPOCH_LIMITS 1000
#define DEBUG_BUILD true

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------

static int framesCounter = 0;
static int finishScreen = 0;

static int ticks = 30;
#define TICKRATE 1000 / 1 / 1000 // update grid X times in a second

double lastTick = 0;
size_t ticksCount;

static Vector2 screenCentre;

static bool paused = false;

// TODO: unite in one buffer to get rid of memcopy
static int nextState[GAME_HEIGHT * GAME_WIDTH];
static int currentState[GAME_HEIGHT * GAME_WIDTH];

static Camera2D camera;
static Vector2 tx_v;
static Texture2D tx;
static Rectangle rc;
static bool running;

static float renderScale = 1.0f;
RenderTexture2D rt2D;

RuleAttrib pStillRule, pBirthRule;

static int nb[NEIGHBORS_COUNT] = {0};

static int rndFig[GRID_X * GRID_Y] = { 0 };

//----------------------------------------------------------------------------------
// Gameplay Functions Definition
//----------------------------------------------------------------------------------

void RandomizeCells() 
{
    for (int i = 0; i < GAME_HEIGHT; i++) {
        for (int j = 0; j < GAME_WIDTH; j++) {
            int rnd = GetRandomValue(0, 2);//int rnd = GetRandomValue(DEAD_CELL, ENEMY_CELL)
            int indx = i * GAME_WIDTH + j;
            nextState[indx] = rnd;
            currentState[indx] = rnd;
        }
    }
}

void RandomizeFigure(int* fig, int type)
{
    for (int i = 0; i < GRID_Y; i++)
    {
        for (int j = 0; j < GRID_Y; j++)
        {
            int rnd = GetRandomValue(0, 1);
            int indx = i * GRID_Y + j;
            if (rnd)
            {
                fig[indx] = type;
            }
        }
    }
}

void RandomizeRule(int* r)
{
    for (int i = 0; i < NEIGHBORS_COUNT; i++)
    {
        r[i] = GetRandomValue(0, 1);
    }
}

void PassGeneration() {
    for (int j = 0; j < GAME_HEIGHT; j++) 
    {
        for (int i = 0; i < GAME_WIDTH; i++) 
        {
            int indx = j * GAME_WIDTH + i;

            GetNeighbors( currentState, (Vector2) { j, i }, &nb, (Vector2) { GAME_WIDTH, GAME_HEIGHT });
 
            nextState[indx] = DEAD_CELL;//default state is death
            
            StayBHV(&nextState[indx], currentState[indx], &nb, pStillRule);
            BirthBHV(&nextState[indx], currentState[indx], &nb, pBirthRule);

        }
    }
    // TODO: delete mem copy
    memcpy(currentState, nextState, sizeof(currentState));
}

void SpawnFigure(int* figure, int x, int y, int type)
{
    for (size_t i = 0; i < GRID_X; i++)
    {
        for (size_t j = 0; j < GRID_Y; j++)
        {
            size_t indx = (y + j) * GAME_WIDTH + x + i;
            int cell = figure[i * GRID_Y + (GRID_Y - 1 - j)];
            if (cell) 
            {
                nextState[indx] = type;
                currentState[indx] = type;
            }
        }
    }
}

void WipeGrid(int wiper)
{
    for (int i = 0; i < GAME_WIDTH; i++) {
        for (int j = 0; j < GAME_HEIGHT; j++) {
            nextState[i * GAME_HEIGHT + j] = wiper;
            currentState[i * GAME_HEIGHT + j] = wiper;
        }
    }
}

void InitCells() {

    WipeGrid(0);
    // Player
    SpawnFigure(&initFigure, 10, 10, PLAYER_CELL);

    // ENEMY
    RandomizeFigure(&rndFig, ENEMY_CELL);
    SpawnFigure(&rndFig, GAME_WIDTH - GRID_X-10, GAME_HEIGHT - GRID_Y-10, ENEMY_CELL);
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
    lastTick = 0;
    ticksCount = 0;
    paused = false;

    int textureWidth = (GAME_WIDTH * CELL_SIZE)>= 1280 ? GAME_WIDTH * CELL_SIZE : 1280;
    int textureHeight = (GAME_HEIGHT * CELL_SIZE) >= 720 ? GAME_HEIGHT * CELL_SIZE : 720;
    int screenWidth =  1280;
    int screenHeight =  720;

    rt2D = LoadRenderTexture(textureWidth * renderScale, textureHeight * renderScale);
    tx = LoadTexture("Resources/Images/cell.png");
    rc = (Rectangle){ 100,200,100,201 };

    // set window size according to grid and cell sizes
    screenCentre = (Vector2){ screenWidth / 2.0f, screenHeight / 2.0f };
    SetWindowSize(screenWidth, screenHeight);

    SetTargetFPS(60);

    InitCells();
    // set virtual camera
    camera.target = (Vector2){ textureWidth / 2.0f, textureHeight / 2.0f +125 };
    camera.offset = (Vector2){ textureWidth / 2.0f, textureHeight / 2.0f, };
    camera.rotation = 0.0f;
    camera.zoom = 0.8f;

    running = true;
}

// Gameplay Screen Update logic
void UpdateGameplayScreen(void)
{
    // Press enter or tap to change to ENDING screen
    if (IsKeyPressed(KEY_ESCAPE))
        finishScreen = SETUP;
    
    if (IsKeyPressed(KEY_SPACE))    paused = !paused;
    if (IsKeyPressed(KEY_ENTER))    RandomizeCells();
    // TODO: redo mouse cam controls
    if (IsKeyDown(KEY_W))   camera.target = (Vector2){ camera.target.x, camera.target.y - 10 };
    if (IsKeyDown(KEY_S))   camera.target = (Vector2){ camera.target.x, camera.target.y + 10 };
    if (IsKeyDown(KEY_A))   camera.target = (Vector2){ camera.target.x - 10, camera.target.y };
    if (IsKeyDown(KEY_D))   camera.target = (Vector2){ camera.target.x + 10, camera.target.y };
        
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
        if (GetTime() - lastTick > (float)1 / ticks)
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
        for (int j = 0; j < GAME_HEIGHT; j++) {
            for (int i = 0; i < GAME_WIDTH; i++) {
                int x = renderScale * i * CELL_SIZE;
                int y = renderScale * j * CELL_SIZE;

                tx_v = (Vector2){ x, y };

                if (nextState[j * GAME_WIDTH + i] == PLAYER_CELL)
                    DrawTextureEx(tx, tx_v, 0, renderScale / M, GREEN); // WTF? Textures problems? //DrawRectangle(x, y, M-5, M-5, GREEN);
                else
                    if(nextState[j * GAME_WIDTH + i] == ENEMY_CELL)
                        DrawTextureEx(tx, tx_v, 0, renderScale / M, RED);
            }
        }
    EndTextureMode();
}

void DrawSummary()
{
    paused = true;
    if (GuiWindowBox((Rectangle) { screenCentre.x, screenCentre.y, 320, 240 }, "Summary"))
    {
        finishScreen = TITLE;
    }

    int* c = CellsCount(currentState, (sizeof(currentState) / sizeof(currentState[0])));

    DrawText(TextFormat("Experiment is over! \n\n \tClear cells - %d \n \tAlien cells - %d", c[1],c[2]), screenCentre.x+35, screenCentre.y+50, 28, DARKGRAY);
}

void RenderGUI() 
{
    DrawText(TextFormat("Epoch - %d", ticksCount), 30, 30, 28, RAYWHITE);
    paused = GuiToggle((Rectangle) { 30, 60, 40, 15 }, "#132#", paused);

    if (GuiButton((Rectangle) { 30, 80, 40, 15 }, "#129#"))
        ticks = ticks < 30 ? 30 : 1;

    if (GuiButton((Rectangle) { 30, 100, 40, 15 }, "#133#"))
        RandomizeCells();

    if (ticksCount > EPOCH_LIMITS)
        DrawSummary();

    if (DEBUG_BUILD)
    {
        if (GuiButton((Rectangle) { 30, 150, 120, 60 }, "#79#"))
            WipeGrid(0);

        if (GuiButton((Rectangle) { 30, 210, 120, 60 }, "#152#"))
            SpawnFigure(&initFigure,GAME_WIDTH / 2, GAME_HEIGHT / 2, PLAYER_CELL);

        char s_str[10] = { 0 }, b_str[10] = { 0 };

        ticks = GuiSlider((Rectangle) { 30, 300, 250, 10 }, "x1", "x30", ticks, 1, 30);

        RuleToText(pStillRule, sizeof(s_str), &s_str);
        RuleToText(pBirthRule, sizeof(b_str), &b_str);

        DrawText(s_str, 250, 10, 28, RED);
        DrawText(b_str, 250, 30, 28, BLUE);
        DrawFPS(10, 10);
    }
}

// Gameplay Screen Draw logic
void DrawGameplayScreen(void)
{
    // Screen draw
    RenderGridToTexture(rt2D);

    BeginMode2D(camera);
        ClearBackground(BLACK);
        DrawTextureEx(rt2D.texture, (Vector2) {10,100}, 0, 1, WHITE);
    EndMode2D();
    
    RenderGUI();
}

// Gameplay Screen Unload logic
void UnloadGameplayScreen(void)
{
    UnloadRenderTexture(rt2D);
    UnloadTexture(tx);
}

// Gameplay Screen should finish?
int FinishGameplayScreen(void)
{
    return finishScreen;
}