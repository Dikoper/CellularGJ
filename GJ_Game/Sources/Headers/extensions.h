/******************************************************************************
*
*   Extensions for engine and game logic
*
******************************************************************************/
#pragma once 

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

typedef int RuleAttrib[NEIGHBORS_COUNT]; //zero index is the root(centre) cell
extern RuleAttrib stay_rule, birth_rule;

//typedef struct Rule {
//    RuleAttrib attribs;
//    void* Behave;
//};

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