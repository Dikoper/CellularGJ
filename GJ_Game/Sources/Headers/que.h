#pragma once
#include "raylib.h"

#define MAX_DEPTH 256

typedef struct
{
    uintptr_t prev;
    uintptr_t next;
}Queue_t;