/**********************************************************************************************
*
*   Screens Functions Declarations (Init, Update, Draw, Unload)
*
**********************************************************************************************/
#ifndef SCREENS_H
#define SCREENS_H

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef enum GameScreen { LOGO = 0, TITLE, OPTIONS, GAMEPLAY, ENDING } GameScreen;

//----------------------------------------------------------------------------------
// Global Variables Declaration (shared by several modules)
//----------------------------------------------------------------------------------
extern GameScreen currentScreen;
extern Font font;
extern Music music;
extern Sound fxSound;
extern const char* GAME_TITLE;

//----------------------------------------------------------------------------------
// Logo Screen Functions Declaration
//----------------------------------------------------------------------------------
    void InitLogoScreen(void);
    void UpdateLogoScreen(void);
    void DrawLogoScreen(void);
    void UnloadLogoScreen(void);
    int FinishLogoScreen(void);

//----------------------------------------------------------------------------------
// Title Screen Functions Declaration
//----------------------------------------------------------------------------------
    void InitTitleScreen(void);
    void UpdateTitleScreen(void);
    void DrawTitleScreen(void);
    void UnloadTitleScreen(void);
    int FinishTitleScreen(void);

//----------------------------------------------------------------------------------
// Options Screen Functions Declaration
//----------------------------------------------------------------------------------
    void InitOptionsScreen(void);
    void UpdateOptionsScreen(void);
    void DrawOptionsScreen(void);
    void UnloadOptionsScreen(void);
    int FinishOptionsScreen(void);

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Declaration
//----------------------------------------------------------------------------------
    void InitGameplayScreen(void);
    void UpdateGameplayScreen(void);
    void DrawGameplayScreen(void);
    void UnloadGameplayScreen(void);
    int FinishGameplayScreen(void);

//----------------------------------------------------------------------------------
// Ending Screen Functions Declaration
//----------------------------------------------------------------------------------
    void InitEndingScreen(void);
    void UpdateEndingScreen(void);
    void DrawEndingScreen(void);
    void UnloadEndingScreen(void);
    int FinishEndingScreen(void);

#endif // SCREENS_H