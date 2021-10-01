/******************************************************************************
*
*   Engine oject type
*   object is an atomic entity of the engine. it has position and activity state
*
******************************************************************************/

#pragma once

#ifndef OBJECTS_H
#define OBJECTS_H

#include "components.h"

#define length(i) (sizeof(i)/sizeof(i[0]))

typedef struct
{
    bool is_active; //state
    int ID;
    const char* name;
    Transform2D transform; // position
    int layer; // depth layer
} Object_t;

int CreateObject();
void DeleteObject(int ID);


static int GetStringHash(char* str, int length)
{
    int prime = 21;
    int hash = 0;

    for (size_t i = 0; i < length; i++)
    {
        hash += str[i] % prime;
    }
    return hash;
};

#endif // OBJECTS_H