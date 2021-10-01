/******************************************************************************
*
* Entry point
*
******************************************************************************/
#include "raylib.h"
#define RAYGUI_IMPLEMENTATION 
#define RAYGUI_SUPPORT_ICONS
#define DEBUG
#include "raygui.h"
#include "./Headers/settings.h"
#include "./Headers/screens.h"
#include "./Headers/objects.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#elif _MSC_VER
    #ifndef DEBUG
    //turn off console in Release mode in MSVC
    #pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
    #endif
#endif

#ifdef DEBUG
#include "./Headers/devtools.h"
#endif // DEBUG

//----------------------------------------------------------------------------------
// Shared Variables Definition (global)
//----------------------------------------------------------------------------------
GameScreen* currentScreen;
Font font = { 0 };
Music music = { 0 };
Sound fxCoin = { 0 };

extern GameScreen main_menu_screen;
extern GameScreen game_screen;
//----------------------------------------------------------------------------------
// Global Variables Definition (local to this module)
//----------------------------------------------------------------------------------
GameScreen* screens[5];

//----------------------------------------------------------------------------------
// Local Functions Declaration
//----------------------------------------------------------------------------------
static void Initialization();
static void GameLoop();
static void Draw();
static void Tick(dt);
static void Deinitialization();

//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------
int main(void)
{
    Initialization();

    GameLoop();

    Deinitialization();

    return 0;
}

//----------------------------------------------------------------------------------
// Module specific Functions Definition
//----------------------------------------------------------------------------------
static void Initialization()
{
    InitWindow(default_settings.screenWidth, default_settings.screenHeight, GAME_TITLE);
    SetTargetFPS(default_settings.framerate);
    SetExitKey(KEY_PAUSE);
    InitAudioDevice();      // Initialize audio device
    font = LoadFont("Resources/Fonts/mecha.png");
    music = LoadMusicStream("Resources/Music/full_theme.ogg");

    SetMusicVolume(music, 1.0f);
    PlayMusicStream(music);

    screens[0] = &main_menu_screen;
    screens[1] = &game_screen;
    currentScreen = screens[0]; // TODO: move screens to manager
    currentScreen->Init();
}

static void Deinitialization()
{
    // Unload current screen data before closing
    currentScreen->Unload();

    // Unload all global loaded data (i.e. fonts) here!
    UnloadFont(font);
    UnloadMusicStream(music);
    UnloadSound(fxCoin);

    CloseAudioDevice();     // Close audio context

    CloseWindow();          // Close window and OpenGL context
}

static void UpdateGame()
{
    Tick(GetFrameTime());
    Draw();
    UpdateMusicStream(music);
};

static void GameLoop() // Main game loop
{
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(UpdateGame, 60, 1);
#endif
    while (!WindowShouldClose())    // Detect window close
    {
        UpdateGame();
    }
}

static void Draw() 
{
    BeginDrawing();

    ClearBackground(BLACK);

    currentScreen->Draw();

#ifdef DEBUG
    DrawDebugInfo(*currentScreen, GetFrameTime());
#endif // DEBUG

    EndDrawing();
}

static void Tick(float dt) 
{
    // TODO: put ticking code here!
    currentScreen->Update();
    int fin = currentScreen->Finish();
    if (fin && currentScreen->state != NONE)
    {
        currentScreen->state = UNLOADING;
        currentScreen->Unload();
        currentScreen->state = NONE;
        currentScreen = screens[fin - 1];

        currentScreen->state = INITIALIZING;
        currentScreen->Init();
    } 
}