/**********************************************************************************************
*
*   Screens Functions Declarations (Init, Update, Draw, Unload)
*
**********************************************************************************************/
#pragma once

#ifndef SCREENS_H
#define SCREENS_H

#define SCREEN_CENTRE (Vector2){GetScreenWidth()/2.0f, GetScreenHeight()/2.0f}

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef enum ScreenType { LOGO = 0, TITLE, OPTIONS, SETUP, GAMEPLAY, ENDING } ScreenType;
typedef enum ScreenState { NONE = 0, INITIALIZING, UPDATING, DRAWING, UNLOADING, FINISHING } ScreenState;
typedef struct GameScreen
{
    ScreenType type;
    ScreenState state;
    // below is a function pointers
    void (*Init)();
    void (*Update)();
    void (*Draw)();
    void (*Unload)();
    int (*Finish)();

} GameScreen;
//----------------------------------------------------------------------------------
// Global Variables Declaration (shared by several modules)
//----------------------------------------------------------------------------------

extern Font font;
extern Music music;
extern Sound fxSound;
extern const char* GAME_TITLE;

#endif // SCREENS_H