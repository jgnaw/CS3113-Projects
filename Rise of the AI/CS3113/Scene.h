#ifndef SCENE_H
#define SCENE_H

#include "Entity.h"
enum EnemyType {SLIME_G, SLIME_P, ORC, BLOOD, DEMON};
struct GameState
{
    Entity *soldier = nullptr;
    Entity *portal = nullptr;
    Entity *hiddenPortal = nullptr;
    Entity *monsters[15];
    int numMonsters = 0;;
    Map    *map = nullptr;
    float iTimer = 0.0f;
    bool airJump = false;
    bool doubleJump = false;

    Music bgm;
    Sound jumpSound;
    Sound hurtSound;

    SceneID nextSceneID;
};

class Scene
{
protected:
    GameState   mGameState;
    Vector2     mOrigin;
    const char *mBGColourHexCode = "#000000";
    static constexpr float INVINCIBLE_TIME = 2.0f;
    static int mLives;
    static int mBossLives;
    static bool trueEnding;

    std::map<Action, std::vector<int>> soldierAnimationAtlas = {
        { P_IDLE,  {  0, 1, 2, 3, 4, 5 } },
        { P_WALKING, { 9, 10, 11, 12, 13, 14, 15, 16 } },
        { P_ATTACK, { 18, 19, 20, 21, 22, 23 }},
        { P_HURT, { 45, 46, 47, 48 }}
    };
    std::map<Action, std::vector<int>> portalAnimationAtlas = {
        { P_IDLE, { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 } }
    };
    std::map<Action, std::vector<int>> slimeAnimationAtlas = {
        { P_IDLE, {4,5,6,7} },
        { P_WALKING, {4,5,6,7} }
    };
    std::map<Action, std::vector<int>> orcAnimationAtlas = {
        { P_IDLE, { 0, 1, 2, 3, 4, 5 }},
        { P_WALKING, { 8, 9, 10, 11, 12, 13, 14, 15 }}
    };
    std::map<Action, std::vector<int>> bloodAnimationAtlas = {
        { P_IDLE, { 0, 1, 2, 3, 4, 5 }},
        { P_WALKING, { 8, 9, 10, 11, 12, 13, 14, 15 }}
    };
    std::map<Action, std::vector<int>> demonAnimationAtlas = {
        { P_IDLE,  {  0, 1, 2, 3, 4, 5 } },
        { P_WALKING, { 8, 9, 10, 11, 12, 13, 14, 15 } },
        { P_ATTACK, { 24, 25, 26, 27, 28, 29, 30 }},
        { P_HURT, { 32, 33, 34, 35 }}
    };

public:
    Scene();
    Scene(Vector2 origin, const char *bgHexCode);
    virtual ~Scene() {}

    virtual void initialise() = 0;
    virtual void processInput() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;
    virtual void shutdown() = 0;

    GameState   getState()           const { return mGameState;        }
    Vector2     getOrigin()          const { return mOrigin;           }
    const char *getBGColourHexCode() const { return mBGColourHexCode; }
    static int getLives() { return mLives; }
};

#endif // SCENE_H