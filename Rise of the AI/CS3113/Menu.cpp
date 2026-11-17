#include "Menu.h"
Menu::Menu() : Scene( {0.0f, 0.0f}, "#000000"){}
Menu::Menu(Vector2 origin, const char *bgHexCode) : Scene(origin, bgHexCode){}
Menu::~Menu(){}

void Menu::initialise()
{
    mGameState.nextSceneID = NO_SCENE;
}
void Menu::processInput()
{
    if (IsKeyPressed(KEY_ENTER)) mGameState.nextSceneID = LEVEL_A;
}
void Menu::update(float deltaTime) {}
void Menu::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));
    const char *title = "Little Giant";
    const char *start = "Press Enter to Start";
    const char *controls = "A/W - move, W - jump";
    DrawText(title, mOrigin.x - MeasureText(title, 50) / 2.0f, mOrigin.y - 200.0f, 50, WHITE);
    DrawText(start, mOrigin.x - MeasureText(start, 20) / 2.0f, mOrigin.y + 20.0f, 20, WHITE);
    DrawText(controls, mOrigin.x - MeasureText(controls, 20) / 2.0f, mOrigin.y + 50.0f, 20, WHITE); 
}
void Menu::shutdown()
{

}