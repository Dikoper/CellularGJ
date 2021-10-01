#pragma once

#ifndef DEV_H
#define DEV_H

Rectangle panelRect = { 0, 30, 200, 250 };
bool isShown = true;

static void DrawDebugInfo(GameScreen gs, float deltaTime)
{
    isShown = GuiToggle((Rectangle) {0,0,40,30},"debug" , isShown);
    
    if (isShown)
    {
        GuiPanel(panelRect);
        SetWindowTitle(FormatText("%s - %d fps", GAME_TITLE,  GetFPS()));
        DrawText(TextFormat("GS - %d", gs.type), 10, 40, 12, DARKGRAY);
        DrawText(TextFormat("dt - %f", deltaTime), 10, 60, 12, DARKGRAY);
    }
}

#endif