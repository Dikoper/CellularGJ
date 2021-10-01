#include "game_screen.h"

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int finishScreen;
Vector2 textPos;
Vector2 offset;

Object_t l_pad, r_pad, ball;

Vector2 ball_velocity;
//----------------------------------------------------------------------------------
// Title Screen Functions Definition
//----------------------------------------------------------------------------------

// Title Screen Initialization logic
void InitGameScreen(void)
{
    // TODO: Initialize TITLE screen variables here!
    finishScreen = 0;

    ball = (Object_t){
        true,
        GetStringHash("ball", length("ball")),
        "ball",
        (Transform2D) { SCREEN_CENTRE, 0.0f, (Vector2){10,10}},
        0
    };

    l_pad = (Object_t){
        true,
        GetStringHash("pad1", length("pad1")),
        "pad1",
        (Transform2D) { (Vector2){ 100, SCREEN_CENTRE.y }, 0.0f, (Vector2){20,80}},
        0
    };

    r_pad = (Object_t){
        true,
        GetStringHash("pad2", length("pad2")),
        "pad2",
        (Transform2D) { (Vector2){ GetScreenWidth() - 100, SCREEN_CENTRE.y }, 0.0f, (Vector2){20,80}},
        0
    };
}

// Title Screen Update logic
void UpdateGameScreen(void)
{

#define pos transform.position
    // TODO: Update Game screen variables here!

    offset = (Vector2){ cosf(GetTime()) * 20, -sinf(GetTime()) *20 };

    // Press enter or tap to change to GAMEPLAY screen
    if (IsKeyPressed(KEY_ENTER))
        finishScreen = 1;   //menu

    if (ball.is_active)
    {
        if (IsKeyPressed(KEY_SPACE))
            ball_velocity = Vector2Add(ball_velocity, (Vector2) { 1, 1 });
        ball.pos = Vector2Add(ball.pos, Vector2Scale(ball_velocity, 10.0f));
    }

    if (l_pad.is_active && r_pad.is_active)
    {
        if (IsKeyDown(KEY_W))
            l_pad.pos.y -= 10.0f;
        if (IsKeyDown(KEY_S))
            l_pad.pos.y += 10.0f;
        if (IsKeyDown(KEY_UP))
            r_pad.pos.y -= 10.0f;
        if (IsKeyDown(KEY_DOWN))
            r_pad.pos.y += 10.0f;
    }

    //bounding check
    {
        if (ball.pos.x > GetScreenWidth() || ball.pos.x < 0)
        {
            ball.pos = SCREEN_CENTRE;
            ball_velocity = Vector2Zero();
        }

        if (ball.pos.y > GetScreenHeight() || ball.pos.y < 0)
        {
            ball_velocity = Vector2Reflect(ball_velocity, (Vector2) { 0, 1 });
        }
    }

    //check collisions
    {
        if (Vector2Distance(ball.pos, l_pad.pos) < 100.0f)
            if (CheckCollisionCircleRec(ball.pos, ball.transform.scale.x,
                (Rectangle) {l_pad.pos.x, l_pad.pos.y, l_pad.transform.scale.x, l_pad.transform.scale.y}
               ))
            {
                ball_velocity = Vector2Reflect(ball_velocity, (Vector2) { 1, 0 });
            }

        if (Vector2Distance(ball.pos, r_pad.pos) < 100.0f)
            if (CheckCollisionCircleRec(ball.pos, ball.transform.scale.x,
                (Rectangle) {r_pad.pos.x, r_pad.pos.y, r_pad.transform.scale.x, r_pad.transform.scale.y}
               ))
            {
                ball_velocity = Vector2Reflect(ball_velocity, (Vector2) { 1, 0 });
            }
    }
}

// Game Screen Draw logic
void DrawGameScreen(void)
{
    // TODO: Draw Game screen here!
    DrawText("Use arrows to move", SCREEN_CENTRE.x, 10, 10, RAYWHITE);
    /*if (point.is_active)
    {
        DrawTextEx(font, FormatText("%c", 64), Vector2Add(point.transform.position, offset), font.baseSize * 8, 3, GRAY);
        DrawCircle(point.transform.position.x, point.transform.position.y, 10, RAYWHITE);
    }*/

    //draw pads
    DrawRectangle(l_pad.pos.x, l_pad.pos.y,
        l_pad.transform.scale.x, l_pad.transform.scale.y,
        RAYWHITE);
    DrawRectangle(r_pad.pos.x, r_pad.pos.y,
        r_pad.transform.scale.x, r_pad.transform.scale.y,
        RAYWHITE);

    //draw ball
    DrawCircle(ball.pos.x, ball.pos.y, ball.transform.scale.x, RAYWHITE);
#ifdef DEBUG
    DrawText(FormatText("%f,%f", ball.pos.x, ball.pos.y), ball.pos.x, ball.pos.y, 2, RED);
#endif

}

// Game Screen Unload logic
void UnloadGameScreen(void)
{
    // TODO: Unload Game screen variables here!
}

// Game Screen should finish?
int FinishGameScreen(void)
{
    return finishScreen;
}