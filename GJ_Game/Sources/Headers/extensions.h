/******************************************************************************
*
*   Extensions for engine and game logic
*
******************************************************************************/
#pragma once 

#define m 2

#define CS 32/m     // cell size
#define GW 48*m     // grid width
#define GH 32*m     // grid height

#define GRID_X 5
#define GRID_Y 8

#define NEIGHBORS_COUNT 9
/* Neighbors layout
* 1|2|3
* 4|0|5
* 6|7|8
*/

enum CellType
{
    PLAYER_CELL=1,
    ENEMY_CELL
};

typedef bool Figure[GRID_X * GRID_Y];

typedef int RuleAttrib[NEIGHBORS_COUNT]; //zero index is the root(centre) cell
extern RuleAttrib stay_rule, birth_rule;
extern Figure initFigure;

//typedef struct Rule {
//    RuleAttrib attribs;
//    void* Behave;
//};

static int WarpCoord(int x, int y, Vector2 size)
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

static int* GetNeighbors(int* buffer, Vector2 pos, int* out, Vector2 warp)
{
    /// OutputIndexesLayout
    /// 1|2|3
    /// 4|0|5
    /// 6|7|8

    int neighbors[NEIGHBORS_COUNT] = { 0 };

    int y = (int)pos.x;
    int x = (int)pos.y;

    out[0] = buffer[WarpCoord(x, y, warp)];
    out[1] = buffer[WarpCoord(x - 1, y - 1, warp)];
    out[2] = buffer[WarpCoord(x, y - 1, warp)];
    out[3] = buffer[WarpCoord(x + 1, y - 1, warp)];
    out[4] = buffer[WarpCoord(x - 1, y, warp)];
    out[5] = buffer[WarpCoord(x + 1, y, warp)];
    out[6] = buffer[WarpCoord(x - 1, y + 1, warp)];
    out[7] = buffer[WarpCoord(x, y + 1, warp)];
    out[8] = buffer[WarpCoord(x + 1, y + 1, warp)];

    return out;
}

static int* CellsCount(int* buffer, int bufferSize) 
{
    int n[10] = { 0 };
    int c = 0;
    for (size_t i = 1; i < bufferSize; i++) //zero index is for sum
    {
        if (buffer[i] == PLAYER_CELL)
        {
            n[PLAYER_CELL]++;
            c++;
        }
        if (buffer[i] == ENEMY_CELL)
        {
            n[ENEMY_CELL]++;
            c++;
        }
    }
    n[0] = c;

    return n;
}

static int* NeighborsCount(int* neighbors)
{
    int n[10] = { 0 };
    int c = 0;
    for (size_t i = 1; i < NEIGHBORS_COUNT; i++) //zero index is for sum
    {
        if (neighbors[i] == PLAYER_CELL)
        {
            n[PLAYER_CELL]++;
            c++;
        }
        if (neighbors[i] == ENEMY_CELL)
        {
            n[ENEMY_CELL]++;
            c++;
        }
    }
    n[0] = c;

    return n;
}

static void StayBHV(int* cell, int prev, int* neighbors, RuleAttrib ra)
{
    int n_count = NeighborsCount(neighbors)[0];
    for (int i = 0; i < NEIGHBORS_COUNT; ++i)
    {
        if (ra[i] >= PLAYER_CELL)
        {
            if (i == n_count)
            {
                *cell = neighbors[0];
                break;
            }
        }
    }
}

static void BirthBHV(int* cell, int prev, int* neighbors, RuleAttrib ra)
{
    int* n_count = NeighborsCount(neighbors);

    for (int i = 0; i < NEIGHBORS_COUNT; ++i)
    {
        if (ra[i] >= PLAYER_CELL)
        {
            if (i == n_count[0])
            {
                *cell = (n_count[PLAYER_CELL] >= n_count[ENEMY_CELL]) ?
                                         PLAYER_CELL : ENEMY_CELL;
                break;
            }
        }
    }
}