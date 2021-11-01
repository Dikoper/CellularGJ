/******************************************************************************
*
*   Engine scene type
*   scene contains objects tree and run dedicated scene script 
*
******************************************************************************/
#pragma once
#include "objects.h"

#define MAX_NODES 256

typedef Object_t ObjectsTree[MAX_NODES];

int AddTreeNode(Object_t o);
void DeleteTreeNode(int ID);