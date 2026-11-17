#include "Boss.h"

Boss::Boss()
    : Scene{ { 0.0f, 0.0f }, "#000000" }
{}

Boss::Boss(Vector2 origin, const char *bgHexCode)
    : Scene{ origin, bgHexCode }
{}
Boss::~Boss() {}

void Boss::initialise()
{
    mGameState.nextSceneID = NO_SCENE;

    mGameState.bgm = LoadMusicStream("assets/Killers.mp3");
    SetMusicVolume(mGameState.bgm, 0.33f);
    PlayMusicStream(mGameState.bgm);

    mGameState.jumpSound = LoadSound("assets/jump.wav");
    mGameState.hurtSound = LoadSound("assets/hurt.wav");
    SetSoundVolume(mGameState.jumpSound, 0.5f);
    
    Vector2 ENEMY_POS[] = {
        { 16.5f*TILE_DIMENSION, 7.5f*TILE_DIMENSION}
    };
    constexpr EnemyType ENEMIES[] = { DEMON };

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
        { mGameState.map->getRightBoundary() - 0.5f*TILE_DIMENSION, mGameState.map->getBottomBoundary() - 2.5f*TILE_DIMENSION}, // position
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
    mGameState.doubleJump = true;
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
    
    mAttackCD = 0.0f;
    mGameState.numMonsters = 1;
    mGameState.monsters[0] = new Entity(
        { mGameState.map->getLeftBoundary() + ENEMY_POS[0].x, mGameState.map->getTopBoundary() + ENEMY_POS[0].y },
        { 600.0f, 600.0f},
        "assets/Demon_A.png",
        ATLAS,
        {6, 8},
        demonAnimationAtlas,
        NPC
    );
    mGameState.monsters[0]->setSpeed(50);
    mGameState.monsters[0]->setColliderDimensions({65, 63});
    mGameState.monsters[0]->setAIType(WANDERER);
    mGameState.monsters[0]->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});
    mGameState.monsters[0]->setRenderOffset({-10.0f, 5.0f});
}

void Boss::processInput()
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
    //press space to attack
    if (IsKeyPressed(KEY_SPACE)) mGameState.soldier->attack();

    // to avoid faster diagonal speed
    if (GetLength(mGameState.soldier->getMovement()) > 1.0f)
        mGameState.soldier->normaliseMovement();
}

void Boss::update(float deltaTime)
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
    if (mBossLives == 0) {
        mGameState.portal->update(
            deltaTime,
            nullptr,
            mGameState.map,
            nullptr,
            0
        );
    }
    //make attacking slower specifically
    if (mGameState.monsters[0]->getAction() == P_ATTACK)
        mGameState.monsters[0]->setFrameSpeed(8);
    else
        mGameState.monsters[0]->setFrameSpeed(14);
        mGameState.monsters[0]->update(
            deltaTime,
            mGameState.soldier,
            mGameState.map,
            nullptr,
            0
        );
    
    mGameState.iTimer = fmax(mGameState.iTimer - deltaTime, 0.0f);
    mAttackCD = fmax(mAttackCD - deltaTime, 0.0f);
    if (mGameState.soldier->overlaps(mGameState.monsters[0]) && mGameState.iTimer == 0.0f)
    {
        mLives--;
        mGameState.soldier->hurt();
        PlaySound(mGameState.hurtSound);
        if (mLives == 0)
            mGameState.nextSceneID = DEFEAT;
        
        mGameState.iTimer = INVINCIBLE_TIME;
    }
    //initiate boss attack towards player if within range
    if (mGameState.monsters[0]->inRange(mGameState.soldier, 110.0f, 60.0f) && mAttackCD == 0.0f &&
       (mGameState.monsters[0]->getAction() != P_ATTACK && mGameState.monsters[0]->getAction() != P_HURT)){
        if (mGameState.monsters[0]->getPosition().x > mGameState.soldier->getPosition().x)
            mGameState.monsters[0]->setDirection(LEFT);
        else mGameState.monsters[0]->setDirection(RIGHT);
        mGameState.monsters[0]->attack();
        mAttackCD = ATTACK_CD;
    }
    //check if boss' attack hits player
    if (mGameState.monsters[0]->getAction() == P_ATTACK && mGameState.iTimer == 0.0f &&
        mGameState.monsters[0]->attackLanded(90.0f, 63.0f, 32.0f, 27, 28, mGameState.soldier)){
            mLives--;
            mGameState.soldier->hurt();
            PlaySound(mGameState.hurtSound);
            if (mLives == 0)
                mGameState.nextSceneID = DEFEAT;
        mGameState.iTimer = INVINCIBLE_TIME;
    }
    //check if player's attack hits the boss
    if (mGameState.soldier->getAction() == P_ATTACK &&
        mGameState.soldier->attackLanded(28.0f, 38.0f, 10.0f, 21, 22, mGameState.monsters[0])){
        mBossLives--;
        if (mGameState.monsters[0]->getAction() != P_ATTACK){
            mGameState.monsters[0]->hurt();
            PlaySound(mGameState.hurtSound);
        }
        //deactivate boss it's defeated
        if (mBossLives == 0)
            mGameState.monsters[0]->deactivate();
    }
    //new ending message when boss killed
    if (mBossLives == 0)
        if (mGameState.soldier->overlaps(mGameState.portal)){
            trueEnding = true;
            mGameState.nextSceneID = VICTORY;
        }       
}

void Boss::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));
    
    mGameState.map->render();
    //onlu render portal if boss defeated
    if (mBossLives == 0)
        mGameState.portal->render();

    mGameState.monsters[0]->render();
    
    mGameState.soldier->render();
    
}

void Boss::shutdown()
{
    delete   mGameState.soldier;
    mGameState.soldier = nullptr;
    delete   mGameState.map;
    mGameState.map = nullptr;
    delete mGameState.monsters[0];
    mGameState.monsters[0] = nullptr;
    
    delete   mGameState.portal;
    mGameState.portal = nullptr;
    mGameState.numMonsters = 0;

    UnloadMusicStream(mGameState.bgm);
    UnloadSound(mGameState.jumpSound);
    UnloadSound(mGameState.hurtSound);
}