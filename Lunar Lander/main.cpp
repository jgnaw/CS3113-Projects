/**
* Author: Josch Wang
* Assignment: Lunar Lander!
* Date due: 10/30/26
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#include "CS3113/Entity.h"

// Global Constants
constexpr int SCREEN_WIDTH  = 1000,
              SCREEN_HEIGHT = 600,
              FPS           = 120;

constexpr Vector2 ORIGIN      = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };

constexpr int   NUMBER_OF_COLUMNS       = 25,
                NUMBER_OF_BLOCKS        = 109;
constexpr float BLOCK_DIMENSION          = 40.0f,
                // in m/ms², since delta time is in ms
                ACCELERATION_OF_GRAVITY = 10.0f,
                THRUST_ACCELERATION     = 17.0f,
                DRAG                    = 2.0f,
                ROTATION_SPEED          = 90.0f,
                FUEL_RATE               = 10.0f,
                PLATFORM_SPEED          = 40.0f,
                FIXED_TIMESTEP          = 1.0f / 60.0f;
constexpr char SSBU_BACKGROUND[] = "assets/SSBU_Background.png";
constexpr char ROB_THRUST[] = "assets/thrust.png";
constexpr char DEATH_EFFECT[] = "assets/death_effect.png";
constexpr char BLASTZONE[] = "assets/blastzone.png";
// Global Variables
AppStatus gAppStatus       = RUNNING;
float     gPreviousTicks   = 0.0f,
          gTimeAccumulator = 0.0f;

Entity *gRob = nullptr;
Entity *gBlocks  = nullptr;
Entity *gPlatform = nullptr;
float gDir = 0.0f;
//max height = 10, min height = 1
int heights[NUMBER_OF_COLUMNS] = {2, 4, 3, 5, 7,
                                  8, 4, 1, 3, 3,
                                  6, 10, 10, 7, 6,
                                  2, 1, 1, 4, 5,
                                  8, 1, 1, 3, 4};
Texture2D gBackground;
Texture2D gThrust;
Texture2D gDeathEffect;
Texture2D gBlastzone;
bool thrustActive = false;
bool roundOver = false;
float fuel = 1000.0f;
int potentialFuelLoss = 0;
int score = 0;
float impactVY = 0.0f;
float impactVX = 0.0f;
int blastzonePos = 0;
const char *landMessage;
// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();

void resetGame() {
    delete gRob;
    if (fuel <= 0.0f) {
        fuel = 1000.0f;
        score = 0;
    }
    gRob = new Entity(
        {ORIGIN.x - 300.0f, ORIGIN.y - 200.0f},
        {32.0f, 40.0f},
        "assets/rob.png",
        PLAYER
    );
    gRob->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});
    blastzonePos = 0;
}
void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Physics");
    gBackground = LoadTexture(SSBU_BACKGROUND);
    gThrust = LoadTexture(ROB_THRUST);
    gDeathEffect = LoadTexture(DEATH_EFFECT);
    gBlastzone = LoadTexture(BLASTZONE);
    /*
        ----------- PROTAGONIST -----------
    */
    gRob = new Entity(
        {ORIGIN.x - 300.0f, ORIGIN.y - 200.0f},
        {32.0f, 40.0f},
        "assets/rob.png",
        PLAYER
    );
    gRob->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});
    //create terrain of blocks using heights array
    gBlocks = new Entity[NUMBER_OF_BLOCKS + 1];
    int blockNum = 0;
    for (int i = 0; i < NUMBER_OF_COLUMNS; i++)
    {
        for (int j = 0; j < heights[i]; j++){
            gBlocks[blockNum].setTexture("assets/stone_block.png");
            gBlocks[blockNum].setEntityType(BLOCK);
            gBlocks[blockNum].setScale({ BLOCK_DIMENSION, BLOCK_DIMENSION });
            gBlocks[blockNum].setColliderDimensions({ BLOCK_DIMENSION, BLOCK_DIMENSION });
            gBlocks[blockNum].setPosition({
                BLOCK_DIMENSION * (i + 0.5f), 
                SCREEN_HEIGHT - BLOCK_DIMENSION * (j + 0.5f)
            });
            blockNum++;
        }
    }
    //initialise platform
    gPlatform = &gBlocks[NUMBER_OF_BLOCKS];
    gPlatform->setTexture("assets/platform.png");
    gPlatform->setEntityType(PLATFORM);
    gPlatform->setScale({ 70.0f, 10.0f });
    gPlatform->setColliderDimensions({ 70.0f, 10.0f });
    gPlatform->setPosition({
        ORIGIN.x + 150.0f, 
        ORIGIN.y
    });
    gPlatform->moveRight();
    
    
    SetTargetFPS(FPS);
}

void processInput()
{
    gDir = 0;
    //enable thrust if available fuel
    if (IsKeyDown(KEY_UP) && fuel > 0.0f) {
        thrustActive = true;
        gRob->setAcceleration({
            THRUST_ACCELERATION * cos((gRob->getAngle() - 90.0f) * 3.14f/180.0f), 
            ACCELERATION_OF_GRAVITY - THRUST_ACCELERATION * sin((90.0f + gRob->getAngle()) * 3.14f/180.0f)
        });
    }
    else {
        thrustActive = false;
        gRob->setAcceleration({gRob->getAcceleration().x, ACCELERATION_OF_GRAVITY});
    }
    //smooth rotation, but land on multipe of 15 degrees on release
    if (IsKeyDown(KEY_RIGHT)) gDir = 1.0f;
    if (IsKeyReleased(KEY_RIGHT)) gRob->setAngle(ceilf(gRob->getAngle() / 15.0f) * 15.0f);
    if (IsKeyDown(KEY_LEFT)) gDir = -1.0f;
    if (IsKeyReleased(KEY_LEFT)) gRob->setAngle(floorf(gRob->getAngle() / 15.0f) * 15.0f);
    //reset round after landing, resets whole game if out of fuel
    if (IsKeyPressed(KEY_R) && roundOver){
        roundOver = false;
        resetGame();
    }
    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
}

void update()
{
    // Delta time
    float ticks     = static_cast<float>(GetTime());
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks  = ticks;

    // Fixed timestep
    deltaTime += gTimeAccumulator;

    if (deltaTime < FIXED_TIMESTEP)
    {
        gTimeAccumulator = deltaTime;
        return;
    }

    while (deltaTime >= FIXED_TIMESTEP)
    {
        if (!roundOver) {
            //update platform   
            if (gPlatform->getPosition().x >= ORIGIN.x + 200.0f) gPlatform->moveLeft();
            else if (gPlatform->getPosition().x < ORIGIN.x + 100.0f) gPlatform->moveRight();
            gPlatform->setPosition({gPlatform->getMovement().x * PLATFORM_SPEED * FIXED_TIMESTEP + gPlatform->getPosition().x, gPlatform->getPosition().y});
            potentialFuelLoss = 0;
            impactVY = gRob->getVelocity().y;
            impactVX = gRob->getVelocity().x;
            gRob->update(FIXED_TIMESTEP, gBlocks, NUMBER_OF_BLOCKS + 1);
            //landing on top of blocks
            if (gRob->isCollidingBottom()){
                bool overhang = false;
                //determine if any part of rob is off a block or platform
                if (gRob->getCollidedEntity() == BLOCK){
                    int left = (int)((gRob->getPosition().x - gRob->getScale().x / 2.0f) / BLOCK_DIMENSION);
                    int right = (int)((gRob->getPosition().x + gRob->getScale().x / 2.0f) / BLOCK_DIMENSION);
                    if (left < 0) left = 0;
                    if (right > NUMBER_OF_COLUMNS - 1) right = NUMBER_OF_COLUMNS - 1;
                    overhang = heights[left] != heights[right];
                }
                else {
                    float left = gPlatform->getPosition().x - gPlatform->getScale().x / 2.0f;
                    float right = gPlatform->getPosition().x + gPlatform->getScale().x / 2.0f;
                    overhang = gRob->getPosition().x - gRob->getScale().x / 2.0f < left ||
                    gRob->getPosition().x + gRob->getScale().x / 2.0f > right;
                }
                //fail if overhang, not upright, or too fast in x or y direction
                if (overhang || gRob->getAngle() != 0.0f || fabs(gRob->getVelocity().x) > 5.0f || impactVY > 15.0f){
                    gRob->deactivate();
                    landMessage = "Mission Failed: We'll get'em next time";
                    potentialFuelLoss = GetRandomValue(150, 200);
                    if (potentialFuelLoss > fuel){
                        potentialFuelLoss = (int)ceilf(fuel);
                    }
                    roundOver = true;
                }
                //medium score
                else if (fabs(gRob->getVelocity().x) > 3.0f || impactVY > 10.0f){
                    score += 50;
                    landMessage = "Mission Accomplished: I mean it's alright like...";
                    potentialFuelLoss = GetRandomValue(100, 150);
                    if (potentialFuelLoss > fuel){
                        potentialFuelLoss = (int)ceilf(fuel);
                    }
                }
                //best score, clean landing
                else{
                    score += 100;
                    landMessage = "Mission Accomplished: W landing";
                }
                roundOver = true;
            }
            //go out of left/right bounds of screen
            else if (gRob->getPosition().x - gRob->getScale().x / 2.0f > SCREEN_WIDTH ||
                gRob->getPosition().x + gRob->getScale().x / 2.0f < 0) {
                if (gRob->getPosition().x < 0) blastzonePos = -1;
                else blastzonePos = 1;
                landMessage = "Mission Failed: How...";
                potentialFuelLoss = GetRandomValue(150, 200);
                if (potentialFuelLoss > fuel){
                    potentialFuelLoss = (int)ceilf(fuel);
                }
                gRob->deactivate();
                roundOver = true;
            }
            //crashed into side of block
            else if (!gRob->isActive()){
                landMessage = "Mission Failed: We'll get'em next time";
                potentialFuelLoss = GetRandomValue(150, 200);
                if (potentialFuelLoss > fuel){
                    potentialFuelLoss = (int)ceilf(fuel);
                }
                roundOver = true;
            }
            //subtract fuel if messed up landing
            fuel -= potentialFuelLoss;
            gRob->setAngle(gRob->getAngle() + gDir * ROTATION_SPEED * FIXED_TIMESTEP);
            if (gRob->getAngle() > 90.0f) gRob->setAngle(90.0f);
            if (gRob->getAngle() < -90.0f) gRob->setAngle(-90.0f);
            //slows down horizontal speed if thrust not active
            if (!thrustActive){
                if (fabs(gRob->getVelocity().x) <= DRAG * FIXED_TIMESTEP){
                    gRob->setAcceleration({-gRob->getVelocity().x / FIXED_TIMESTEP, gRob->getAcceleration().y});
                }
                else if (gRob->getVelocity().x > 0){
                    gRob->setAcceleration({-DRAG, gRob->getAcceleration().y});
                }
                else if (gRob->getVelocity().x < 0){
                    gRob->setAcceleration({DRAG, gRob->getAcceleration().y});
                }
            }
            //decrease fuel while thrust active
            else if (gRob->isActive()){
                fuel -= FUEL_RATE * FIXED_TIMESTEP;
            }
            //update blocks
            for (int i = 0; i < NUMBER_OF_BLOCKS; i++)
                gBlocks[i].update(FIXED_TIMESTEP, nullptr, 0);
        }

        deltaTime -= FIXED_TIMESTEP;
    }
    
    gTimeAccumulator = deltaTime;
}

void render()
{
    BeginDrawing();
    //background texture
    Rectangle bgArea = {
        0.0f, 0.0f,
        static_cast<float>(gBackground.width),
        static_cast<float>(gBackground.height)
    };
    Rectangle bgDestinationArea = {
        0.0f,
        0.0f,
        static_cast<float>(SCREEN_WIDTH),
        static_cast<float>(SCREEN_HEIGHT)
    };
    DrawTexturePro(
        gBackground,
        bgArea, bgDestinationArea, {0.0f, 0.0f},
        0.0f, WHITE
    );
    //thrust texture, follows rob
    Rectangle thrustArea = {
        0.0f, 0.0f,
        static_cast<float>(gThrust.width),
        static_cast<float>(gThrust.height)
    };
    Rectangle thrustDestinationArea = {
        gRob->getPosition().x,
        gRob->getPosition().y,
        40.0f,
        65.0f
    };
    Vector2 thrustOffset = {
        gRob->getScale().x / 2.0f,
        -gRob->getScale().y / 2.0f
    };
    //explosion effect, for failed landings
    Rectangle deathArea = {
        0.0f, 0.0f,
        static_cast<float>(gDeathEffect.width),
        static_cast<float>(gDeathEffect.height)
    };
    Rectangle deathDestinationArea = {
        gRob->getPosition().x,
        gRob->getPosition().y,
        50.0f,
        40.0f
    };
    Vector2 deathOffset = {
        gRob->getScale().x / 2.0f,
        gRob->getScale().y / 2.0f
    };
    //blast zone death effect texture, rob goes off to the side
    Rectangle bzArea = {
        0.0f, 0.0f,
        static_cast<float>(blastzonePos * gBlastzone.width),
        static_cast<float>(gBlastzone.height)
    };
    Rectangle bzDestinationArea = {
        blastzonePos == -1 ? 0 : static_cast<float>(SCREEN_WIDTH),
        gRob->getPosition().y,
        200.0f,
        100.0f
    };
    //adjust direction of texture depending on where rob goes off screen
    Vector2 bzOffset = {
        blastzonePos == -1 ? gRob->getScale().x / 2.0f : 200 - gRob->getScale().x / 2.0f,
        gRob->getScale().y
    };
    if (gRob->isActive()) {
        //render rob if actively moving or landed safely
        gRob->render();
        if (!roundOver){
            //draw thrust texture if thrust is active and round is still going
            if (thrustActive) {
                DrawTexturePro(
                    gThrust,
                    thrustArea, thrustDestinationArea, thrustOffset,
                    gRob->getAngle(), WHITE
                );
            }
        }
    }
    //draw blast zone death texture if rob goes off to the side
    else if (gRob->getPosition().x - gRob->getScale().x / 2.0f > SCREEN_WIDTH ||
            gRob->getPosition().x + gRob->getScale().x / 2.0f < 0) {
            DrawTexturePro(
                gBlastzone,
                bzArea, bzDestinationArea, bzOffset,
                0.0f, WHITE
            );
    }
    //draw explosion effect if rob dies
    else if (!gRob->isActive()) {
        DrawTexturePro(
            gDeathEffect,
            deathArea, deathDestinationArea, deathOffset,
            0.0f, WHITE
        );
    }
    //display mission success/failed message
    if (roundOver) {
        DrawText(landMessage, SCREEN_WIDTH / 2.0f -  MeasureText(landMessage, 20) / 2.0f, 0, 20, WHITE);
        if (potentialFuelLoss != 0) {
            //display fuel lost if mission failed
            const char *fuelLost = TextFormat("%i fuel units lost", potentialFuelLoss);
            DrawText(fuelLost, SCREEN_WIDTH / 2.0f -  MeasureText(fuelLost, 20) / 2.0f, 30, 20, WHITE);
        }
    }
    //render blocks and platform
    for (int i = 0; i < NUMBER_OF_BLOCKS; i++) gBlocks[i].render();
    gPlatform->render();
    //render statistics (score, fuel, x and y velocities)
    const char *velocityY = TextFormat("Vertical Velocity: %i", int(impactVY));
    const char *velocityX = TextFormat("Horizontal Velocity: %i", int(impactVX));
    const char *fuelCount = TextFormat("Fuel: %i", (int)ceilf(fuel));
    const char *scoreTracker = TextFormat("Score: %i", score);
    const char *gameOver = "Game Over";
    DrawText(velocityX, SCREEN_WIDTH - MeasureText(velocityX, 20), 0, 20, WHITE);
    DrawText(velocityY, SCREEN_WIDTH - MeasureText(velocityY, 20), 30, 20, WHITE);
    DrawText(fuelCount, 0, 30, 20, WHITE);
    DrawText(scoreTracker, 0, 0, 20, WHITE);
    //display game over message if out of fuel
    if (fuel <= 0.0f) {
        DrawText(gameOver, SCREEN_WIDTH / 2.0f - MeasureText(gameOver, 30) / 2.0f, 100, 30, WHITE);
    }
    EndDrawing();
}

void shutdown()
{
    // Unload the textures while the window (GL context) still exists
    delete gRob;
    delete[] gBlocks;
    UnloadTexture(gBackground);
    UnloadTexture(gThrust);
    UnloadTexture(gDeathEffect);
    UnloadTexture(gBlastzone);
    CloseWindow();
}

int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();
        render();
    }

    shutdown();

    return 0;
}
