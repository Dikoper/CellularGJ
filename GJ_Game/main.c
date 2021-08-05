/*
	GOOD EXAMPLES:
	-basic_game_template(базовый шаблон)
    -core2D_camera_platformer(следящая камера)
    -core_sorage_value(сохрвненние данных в файл)
    -shaders_spotlight(освещение объекта через альфа канал)
    -shapes_collision_area
    -texture_background_scrolling
    -texture_bunnymark
    -texture_draw_tiled
    -texture_particle_blending
    -textures_rectangle
*/


#include <stdio.h>
#include "raymath.h"

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

    SetTargetFPS(30);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
	while (!WindowShouldClose())    // Detect window close button or ESC key
	{
		// Update
		//----------------------------------------------------------------------------------
		// TODO: Update your variables here
		float _t = 128 * sinf(GetTime());

		Color clr = (Color){ 128 + _t, 128 + _t, 128 - _t, 255};
		//----------------------------------------------------------------------------------

		// Draw
		//----------------------------------------------------------------------------------
		BeginDrawing();

		ClearBackground(clr);

        DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);
		DrawFPS(0,0);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}