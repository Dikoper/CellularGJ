/******************************************************************************
*
*   CelluLar game by Dikoper & Mikl
*
*   Press
*       Enter to randomize the cells
*       Space to pause/unpause
*       G to pass one generation (if paused)
*       F1 to turn on/off the FPS counter
*       Q to quit (I'm just used to having this while testing)
*
******************************************************************************/
#include "raylib.h"
#define RAYGUI_IMPLEMENTATION 
#define RAYGUI_SUPPORT_ICONS
#include "raygui.h"

#include "./Headers/screens.h"

//----------------------------------------------------------------------------------
// Shared Variables Definition (global)
//----------------------------------------------------------------------------------
GameScreen currentScreen = 0;
Font font = { 0 };
Music music = { 0 };
Sound fxCoin = { 0 };
const char* GAME_TITLE = "CELL SHOCK";

//----------------------------------------------------------------------------------
// Global Variables Definition (local to this module)
//----------------------------------------------------------------------------------
static const int screenWidth = 1280;
static const int screenHeight = 720;

// Required variables to manage screen transitions (fade-in, fade-out)
static float transAlpha = 0.0f;
static bool onTransition = false;
static bool transFadeOut = false;
static int transFromScreen = -1;
static int transToScreen = -1;

//----------------------------------------------------------------------------------
// Local Functions Declaration
//----------------------------------------------------------------------------------
static void UpdateDrawFrame(void);          // Update and draw one frame
static void ScreenUpdater(GameScreen gs);
static void ChangeToScreen(int screen);     // Change to screen, no transition effect
static void TransitionToScreen(int screen); // Request transition to next screen
static void UpdateTransition(void);         // Update transition effect
static void DrawTransition(void);           // Draw transition effect (full-screen rectangle)
//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //---------------------------------------------------------

    InitWindow(screenWidth, screenHeight, GAME_TITLE);
    SetExitKey(KEY_PAUSE);
    InitAudioDevice();      // Initialize audio device
    font = LoadFont("Resources/Fonts/mecha.png");
    music = LoadMusicStream("Resources/Music/full_theme.ogg");

    SetMusicVolume(music, 1.0f);
    PlayMusicStream(music);

    currentScreen = TITLE;
    InitTitleScreen();
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        UpdateDrawFrame();
        UpdateMusicStream(music);
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    // Unload current screen data before closing
    switch (currentScreen)
    {
        case TITLE: UnloadTitleScreen(); break;
        case GAMEPLAY: UnloadGameplayScreen(); break;
        case SETUP: UnloadSetupScreen(); break;
    default: break;
    }

    // Unload all global loaded data (i.e. fonts) here!
    UnloadFont(font);
    UnloadMusicStream(music);
    UnloadSound(fxCoin);

    CloseAudioDevice();     // Close audio context

    CloseWindow();          // Close window and OpenGL context

    return 0;
}

//----------------------------------------------------------------------------------
// Module specific Functions Definition
//----------------------------------------------------------------------------------

// Update and draw game frame
static void UpdateDrawFrame(void)
{
    BeginDrawing();
    ClearBackground(RAYWHITE);

    ScreenUpdater(currentScreen);

    switch (currentScreen)
    {
        case TITLE: DrawTitleScreen(); break;
        case GAMEPLAY: DrawGameplayScreen(); break;
        case SETUP: DrawSetupScreen(); break;
        default: break;
    }

    if (onTransition) DrawTransition();

    EndDrawing();
}

static void ScreenUpdater(GameScreen gs)
{
    if (!onTransition)
    {
        switch (gs)
        {
            case TITLE:
            {
                UpdateTitleScreen();

                //if (FinishTitleScreen() == 1) TransitionToScreen(OPTIONS);
                //else 
                if (FinishTitleScreen() == 2) TransitionToScreen(SETUP);

            } break;
            case SETUP:
            {
                UpdateSetupScreen();

                if (FinishSetupScreen() == 1) TransitionToScreen(GAMEPLAY);

            } break;
            case GAMEPLAY:
            {
                UpdateGameplayScreen();

                if (FinishGameplayScreen() == 1) TransitionToScreen(TITLE);
                else 
                if (FinishGameplayScreen() == 2) TransitionToScreen(SETUP);

            } break;
            default: break;
        }
    }
    else UpdateTransition();    // Update transition (fade-in, fade-out)
}

// Change to next screen, no transition
static void ChangeToScreen(int screen)
{
    // Unload current screen
    switch (currentScreen)
    {
        case TITLE: UnloadTitleScreen(); break;
        case GAMEPLAY: UnloadGameplayScreen(); break;
        case SETUP: UnloadSetupScreen(); break;
        default: break;
    }

    // Init next screen
    switch (screen)
    {
        case TITLE: InitTitleScreen(); break;
        case GAMEPLAY: InitGameplayScreen(); break;
        case SETUP: InitSetupScreen(); break;
        default: break;
    }

    currentScreen = screen;
}

// Request transition to next screen
static void TransitionToScreen(int screen)
{
    onTransition = true;
    transFadeOut = false;
    transFromScreen = currentScreen;
    transToScreen = screen;
    transAlpha = 0.0f;
}

// Update transition effect
static void UpdateTransition(void)
{
    if (!transFadeOut)
    {
        transAlpha += 0.5f;

        // NOTE: Due to float internal representation, condition jumps on 1.0f instead of 1.05f
        // For that reason we compare against 1.01f, to avoid last frame loading stop
        if (transAlpha > 1.01f)
        {
            transAlpha = 1.0f;

            // Unload current screen
            switch (transFromScreen)
            {
                case TITLE: UnloadTitleScreen(); break;
                case GAMEPLAY: UnloadGameplayScreen(); break;
                case SETUP: UnloadSetupScreen(); break;
                default: break;
            }

            // Load next screen
            switch (transToScreen)
            {
                case TITLE: InitTitleScreen(); break;
                case GAMEPLAY: InitGameplayScreen(); break;
                case SETUP: InitSetupScreen(); break;
                default: break;
            }

            currentScreen = transToScreen;

            // Activate fade out effect to next loaded screen
            transFadeOut = true;
        }
    }
    else  // Transition fade out logic
    {
        transAlpha -= 0.2f;

        if (transAlpha < -0.01f)
        {
            transAlpha = 0.0f;
            transFadeOut = false;
            onTransition = false;
            transFromScreen = -1;
            transToScreen = -1;
        }
    }
}

// Draw transition effect (full-screen rectangle)
static void DrawTransition(void)
{
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, transAlpha));

}