/******************************************************************************
*
*   Game components
*
******************************************************************************/
#pragma once

#ifndef RAYLIB_H
    #include "raylib.h"
#endif // !RAYLIB_H

#include "physac.h"

typedef struct Transform2D
{
	Vector2 position;
	float rotation; //in degrees
	Vector2 scale; // size for rect [pos.x, pos.y, scale.x, scale.y]
} Transform2D;

typedef struct AudioSlot 
{
	Sound clip;
	const char* file_path;
} AudioSlot;

////__Test example
//typedef struct object
//{
//	Transform2D transform;
//	PhysicsBodyData body;
//	Model shape;
//};
////__