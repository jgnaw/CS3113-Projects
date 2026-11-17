#include "Victory.h"
Victory::Victory() : Scene( {0.0f, 0.0f}, "#000000"){}
Victory::Victory(Vector2 origin, const char *bgHexCode) : Scene(origin, bgHexCode){}
Victory::~Victory(){}

void Victory::initialise()
{
    mGameState.nextSceneID = NO_SCENE;
}
void Victory::processInput()
{
    //reset game
    if (IsKeyPressed(KEY_ENTER)) {
        mLives = 5;
        mBossLives = 7;
        mGameState.nextSceneID = MENU;
    }
}
void Victory::update(float deltaTime) {}
void Victory::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));
    //different messages based on route taken
    const char *congrats = "Congratulations!";
    const char *ending1 = "Is the journey really over?";
    const char *ending2 = "On to the next adventure!";
    
    DrawText(congrats, mOrigin.x - MeasureText(congrats, 35) / 2.0f, mOrigin.y - 20, 35, WHITE);
    //didn't defeat boss
    if (trueEnding)
        DrawText(ending2, mOrigin.x - MeasureText(ending2, 20) / 2.0f, mOrigin.y + 20.0f, 20, WHITE); 
    //defeated boss
    if (!trueEnding)
        DrawText(ending1, mOrigin.x - MeasureText(ending1, 20) / 2.0f, mOrigin.y + 20.0f, 20, WHITE);    
}
void Victory::shutdown()
{

}