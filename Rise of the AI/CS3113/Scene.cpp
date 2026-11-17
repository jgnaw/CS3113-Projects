#include "Scene.h"

Scene::Scene() : mGameState{}, mOrigin{} {}

Scene::Scene(Vector2 origin, const char *bgHexCode)
    : mGameState{}, mOrigin{ origin }, mBGColourHexCode{ bgHexCode }
{}

int Scene::mLives = 5;
int Scene::mBossLives = 7;
bool Scene::trueEnding = false;