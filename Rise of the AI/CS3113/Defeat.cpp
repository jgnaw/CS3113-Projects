#include "Defeat.h"
Defeat::Defeat() : Scene( {0.0f, 0.0f}, "#000000"){}
Defeat::Defeat(Vector2 origin, const char *bgHexCode) : Scene(origin, bgHexCode){}
Defeat::~Defeat(){}

void Defeat::initialise()
{
    mGameState.nextSceneID = NO_SCENE;
}
void Defeat::processInput()
{
    //reset game
    if (IsKeyPressed(KEY_ENTER)) {
        mLives = 5;
        mBossLives = 7;
        mGameState.nextSceneID = MENU;
    }
}
void Defeat::update(float deltaTime) {}
void Defeat::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));
    const char *defeat = "Game Over";
    const char *backup = "You can't give up yet";
    //losing message
    DrawText(defeat, mOrigin.x - MeasureText(defeat, 50) / 2.0f, mOrigin.y - 200, 50, WHITE);
    DrawText(backup, mOrigin.x - MeasureText(backup, 20) / 2.0f, mOrigin.y, 20, WHITE);    
}
void Defeat::shutdown()
{

}