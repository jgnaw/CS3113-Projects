#ifndef BOSS_H
#define BOSS_H

#include "LevelC.h"

class Boss : public Scene
{
private:
    //boss can only attack every 8 seconds max
    float mAttackCD;;
    static constexpr float ATTACK_CD = 8.0f;
    static constexpr int LEVEL_WIDTH  = 20,
                         LEVEL_HEIGHT = 10;
    unsigned int mLEVEL_DATA[LEVEL_HEIGHT * LEVEL_WIDTH] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0,0,0,0,0,0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0,0,0,0,0,0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0,0,0,0,0,0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0,0,0,0,0,0,
    0, 0, 5, 5, 5, 0, 0, 0, 5, 5, 5, 5, 0, 0,0,5,5,5,0,0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0,0,0,0,0,0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0,0,0,0,0,0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0,0,0,0,0,0,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,5,5,5,5,5,5,
    21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,21,21,21,21,21,21
};

public:
    static constexpr float TILE_DIMENSION = 50.0f,
                           // in m/ms², since delta time is in ms
                           ACCELERATION_OF_GRAVITY = 981.0f;            

    Boss();
    Boss(Vector2 origin, const char *bgHexCode);
    ~Boss();

    void initialise() override;
    void processInput() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

#endif // Boss_H