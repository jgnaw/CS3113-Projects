#include "LevelA.h"

LevelA::LevelA()
    : Scene{ { 0.0f, 0.0f }, "#000000" }
{}

LevelA::LevelA(Vector2 origin, const char *bgHexCode)
    : Scene{ origin, bgHexCode }
{}
LevelA::~LevelA() {}

void LevelA::initialise()
{
    mGameState.nextSceneID = NO_SCENE;

    mGameState.bgm = LoadMusicStream("assets/Morning.mp3");
    SetMusicVolume(mGameState.bgm, 0.33f);
    PlayMusicStream(mGameState.bgm);

    mGameState.jumpSound = LoadSound("assets/jump.wav");
    mGameState.hurtSound = LoadSound("assets/hurt.wav");
    SetSoundVolume(mGameState.jumpSound, 0.5f);
    
    //determine all enemy positions and enemy types beforehand
    Vector2 ENEMY_POS[] = {
        { 40.5f*TILE_DIMENSION, 5.5f*TILE_DIMENSION},
        { 57.5f*TILE_DIMENSION, 5.5f*TILE_DIMENSION},
        { 54.5f*TILE_DIMENSION, 3.5f*TILE_DIMENSION},
        { 12.5f*TILE_DIMENSION, 6.5f*TILE_DIMENSION},
        { 17.5f*TILE_DIMENSION, 5.5f*TILE_DIMENSION},
        { 32.5f*TILE_DIMENSION, 5.5f*TILE_DIMENSION}
    };
    constexpr EnemyType ENEMIES[] = { SLIME_P, SLIME_P, SLIME_G, SLIME_G, SLIME_G, SLIME_G};

    mGameState.map = new Map(
        LEVEL_WIDTH,                            // map grid cols
        LEVEL_HEIGHT,                           // map grid rows
        mLEVEL_DATA, // grid data
        "assets/world_tileset.png",             // texture filepath
        TILE_DIMENSION,                         // tile size
        16,                                      // texture cols
        16,                                      // texture rows
        mOrigin                                  // in-game origin
    );
    
    
    mGameState.portal = new Entity(
        { mGameState.map->getRightBoundary() - 0.5f*TILE_DIMENSION, mGameState.map->getTopBoundary() + 1.5f*TILE_DIMENSION}, // position
        { TILE_DIMENSION, TILE_DIMENSION  },           // scale
        "assets/portalRings1.png",                   // texture file address
        ATLAS,                                    // single image or atlas?
        { 5, 4 },                         // atlas dimensions
        portalAnimationAtlas,                    // actual atlas
        PORTAL                                    // entity type
    );

    /*
        ----------- PROTAGONIST -----------
    */

    // Assets from @see https://sscary.itch.io/the-adventurer-female
    mGameState.soldier = new Entity(
        { mGameState.map->getLeftBoundary() + 2.5f*TILE_DIMENSION, mGameState.map->getTopBoundary() + 4.5f*TILE_DIMENSION}, // position
        { 200.0f , 200.0f },           // scale
        "assets/Soldier.png",                   // texture file address
        ATLAS,                                    // single image or atlas?
        { 7, 9 },                         // atlas dimensions
        soldierAnimationAtlas,                    // actual atlas
        PLAYER                                    // entity type
    );

    mGameState.soldier->setJumpingPower(550.0f);
    mGameState.soldier->setColliderDimensions({
        23.0f,
        35.0f
    });
    mGameState.soldier->setAcceleration({ 0.0f, ACCELERATION_OF_GRAVITY });
    mGameState.soldier->setRenderOffset( {0.0f, 4.0f});
    
    mGameState.numMonsters = 6;
    //create each monster based on it's type
    for (int i = 0; i < mGameState.numMonsters; i++){
        if (ENEMIES[i] == SLIME_P){
            mGameState.monsters[i] = new Entity(
                { mGameState.map->getLeftBoundary() + ENEMY_POS[i].x, mGameState.map->getTopBoundary() + ENEMY_POS[i].y },
                { 70.0f, 60.0f},
                "assets/slime_purple.png",
                ATLAS,
                {3, 4},
                slimeAnimationAtlas,
                NPC
            );
            mGameState.monsters[i]->setSpeed(125);
        }
        else{
            mGameState.monsters[i] = new Entity(
                { mGameState.map->getLeftBoundary() + ENEMY_POS[i].x, mGameState.map->getTopBoundary() + ENEMY_POS[i].y },
                { 70.0f, 60.0f},
                "assets/slime_green.png",
                ATLAS,
                {3, 4},
                slimeAnimationAtlas,
                NPC
            );
            mGameState.monsters[i]->setSpeed(100);
        }
        mGameState.monsters[i]->setColliderDimensions({
            40.0f,
            30.0f
        });
        mGameState.monsters[i]->setAIType(FOLLOWER);
        mGameState.monsters[i]->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});
        mGameState.monsters[i]->setRenderOffset({0.0f, (mGameState.monsters[i]->getColliderDimensions().y) / 2.0f - mGameState.monsters[i]->getScale().y / 2.0f});
    }
}

void LevelA::processInput()
{
    mGameState.soldier->resetMovement();
    if (IsKeyDown(KEY_A)) mGameState.soldier->moveLeft();
    else if (IsKeyDown(KEY_D)) mGameState.soldier->moveRight();

    if (IsKeyPressed(KEY_W) && (mGameState.soldier->isCollidingBottom() || 
        mGameState.airJump && mGameState.doubleJump))
    {   
        if (!mGameState.soldier->isCollidingBottom()) mGameState.airJump = false;
        mGameState.soldier->jump();
        PlaySound(mGameState.jumpSound);
        
    }
    // to avoid faster diagonal speed
    if (GetLength(mGameState.soldier->getMovement()) > 1.0f)
        mGameState.soldier->normaliseMovement();
}

void LevelA::update(float deltaTime)
{
    UpdateMusicStream(mGameState.bgm);

    mGameState.soldier->update(
        deltaTime,  // delta time / fixed timestep
        nullptr,         // player
        mGameState.map,         // map
        nullptr,    // collidable entities
        0  // col. entity count
    );
    if (mGameState.soldier->isCollidingBottom()) mGameState.airJump = true;
    mGameState.portal->update(
        deltaTime,
        nullptr,
        mGameState.map,
        nullptr,
        0
    );
    for (int i = 0; i < mGameState.numMonsters; i++){
        mGameState.monsters[i]->update(
            deltaTime,
            mGameState.soldier,
            mGameState.map,
            nullptr,
            0
        );
    }
    mGameState.iTimer = fmax(mGameState.iTimer - deltaTime, 0.0f);
    //give 2 seconds of invincibility on hit
    for (int i = 0; i < mGameState.numMonsters; i++){
        if (mGameState.soldier->overlaps(mGameState.monsters[i]) && mGameState.iTimer == 0.0f)
        {printf("hit\n");
            mLives--;
            mGameState.soldier->hurt();
            PlaySound(mGameState.hurtSound);
            if (mLives == 0) {
                mGameState.nextSceneID = DEFEAT;
            }
            mGameState.iTimer = INVINCIBLE_TIME;
        }
    }
    if (mGameState.soldier->overlaps(mGameState.portal))
        mGameState.nextSceneID = LEVEL_B;
}

void LevelA::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));
    
    mGameState.map->render();
    mGameState.portal->render();
    for (int i = 0; i < mGameState.numMonsters; i++){
        mGameState.monsters[i]->render();
    }
    mGameState.soldier->render();
    
}

void LevelA::shutdown()
{
    delete   mGameState.soldier;
    mGameState.soldier = nullptr;
    delete   mGameState.map;
    mGameState.map = nullptr;
    for (int i = 0; i < mGameState.numMonsters; i++){
        delete mGameState.monsters[i];
        mGameState.monsters[i] = nullptr;
    }
    delete   mGameState.portal;
    mGameState.portal = nullptr;
    mGameState.numMonsters = 0;

    UnloadMusicStream(mGameState.bgm);
    UnloadSound(mGameState.jumpSound);
    UnloadSound(mGameState.hurtSound);
}