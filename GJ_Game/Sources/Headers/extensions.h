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

typedef int RuleAttrib[NEIGHBORS_COUNT]; //zero index is the root(centre) cell

typedef struct Rule {
    RuleAttrib attribs;
    void* Behave;
};

extern RuleAttrib stay_rule, birth_rule, kill_rule;

// TODO : move all rules code here

static void StayBHV(int* cell, int prev, int* neighbors, RuleAttrib ra)
{
    int n_count = 0;

    //TraceLog(LOG_INFO, TextFormat("Cell is : %d", *cell));

    for (size_t i = 1; i < NEIGHBORS_COUNT; i++) //zero index is for cell itself
    {
//        TraceLog(LOG_INFO, TextFormat("\n\t%d", neighbors[i]));
        if (neighbors[i] >= 1)
            n_count++;
    }

    for (int i = 0; i < NEIGHBORS_COUNT; ++i)
    {
        if (ra[i] >= 1)
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
    int n_count = 0;

    for (size_t i = 1; i < NEIGHBORS_COUNT; i++)
    {
        if (neighbors[i] >= 1) n_count++;
    }

    for (int i = 0; i < NEIGHBORS_COUNT; ++i)
    {
        if (ra[i] >= 1)
        {
            if (i == n_count)
            {
                 *cell = 1;
                break;
            }
        }
    }
}

static void KillBHV(int* cell, int prev, int neighbors, RuleAttrib ra)
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