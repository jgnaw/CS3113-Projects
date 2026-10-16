/**
* Author: Josch Wang
* Assignment: Simple 2D Scene
* Date due: 10/5/26
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#include "CS3113/cs3113.h"
#include <math.h>

// Global Constants
constexpr int   SCREEN_WIDTH  = 1600 / 2,
                SCREEN_HEIGHT = 900 / 2,
                FPS           = 60,
                FRAME_LIMIT   = 400;
constexpr float MAX_AMP       = 50.0f;

constexpr char    BG_COLOUR[] = "#000000";
constexpr Vector2 ORIGIN      = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };

//size to make all the textures squares
constexpr float TEXTURE_SIZE = SCREEN_HEIGHT * 0.2f;
                

//starting positions set relative to the screen size
constexpr Vector2 MARIO_START = { SCREEN_WIDTH * 0.35f, SCREEN_HEIGHT * 0.4f };
constexpr Vector2 BAT_START   = { SCREEN_WIDTH * 0.35f, SCREEN_HEIGHT * 0.4f };
constexpr Vector2 SONIC_START = { SCREEN_WIDTH * 0.65f, SCREEN_HEIGHT * 0.4f };



constexpr char BATTLEFIELD[] = "assets/SSBU-Battlefield.png";
constexpr char KILL_SPARK[] = "assets/Kill-Spark.png";
constexpr char MARIO[] = "assets/Mario-Model.png";
constexpr char SONIC[] = "assets/Sonic-Model.png";
constexpr char HOME_RUN_BAT[] = "assets/SSBUHomeRunBat.png";

// Global Variables
AppStatus gAppStatus     = RUNNING;
float     gMarioJumpHeight = SCREEN_HEIGHT * 0.15f,
          gBatOrbitAngle = 0.0f,
          gBatAngle      = 45.0f,
          gSonicAngle    = 0.0f;
Vector2   gPositionMario = {},
          gPositionBat   = {},
          gPositionSonic = {};
Vector2   gBatScale      = {TEXTURE_SIZE, TEXTURE_SIZE};

float gPreviousTicks;
int frames = 0;
//calculations for parabolic movement
float t = 150.0f / FPS;
float gMarioSpeedx = SCREEN_WIDTH * 0.23f / t;
float gMarioGravity = 8.0f * gMarioJumpHeight / t / t;
float gMarioSpeedy = -4.0f * gMarioJumpHeight / t;
Texture2D gActiveBG;
Texture2D gBattlefieldTexture;
Texture2D gKillSparkTexture;
Texture2D gMario;
Texture2D gBat;
Texture2D gSonic;

// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();

// Function Definitions
void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Textures");

    gBattlefieldTexture = LoadTexture(BATTLEFIELD);
    gKillSparkTexture = LoadTexture(KILL_SPARK);
    gActiveBG = gBattlefieldTexture;
    gMario = LoadTexture(MARIO);
    gBat = LoadTexture(HOME_RUN_BAT); 
    gSonic = LoadTexture(SONIC);

    gPositionMario = MARIO_START;
    gPositionBat   = BAT_START;
    gPositionSonic = SONIC_START;

    //initalize gPreviousTicks to get accurate
    //delta time for first cycle
    gPreviousTicks = GetTime();
    SetTargetFPS(FPS);
}

void processInput()
{
    if (WindowShouldClose()) gAppStatus = TERMINATED;
}

void update()
{
    //calculate delta time
    float ticks = GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;

    //separate different parts of the scene
    frames++;
    if (frames <= 150)
    {   //mario and bat move in parabolic path
        gPositionMario.x += gMarioSpeedx * deltaTime;
        gMarioSpeedy += gMarioGravity * deltaTime;
        gPositionMario.y += gMarioSpeedy * deltaTime;

        gPositionBat.x += gMarioSpeedx * deltaTime;
        gPositionBat.y += gMarioSpeedy * deltaTime;
    }
    else if (frames < 200)
    {
        //bat scales up and back to normal while orbiting
        //around small radius
        gBatOrbitAngle += 360 * FPS / 50.0f * deltaTime;
        gBatAngle += 360.0f * FPS / 50.0f * deltaTime;
        gBatScale = {
            TEXTURE_SIZE + MAX_AMP * (1-cosf(gBatOrbitAngle * 3.14/180)) * 0.5f,
            TEXTURE_SIZE + MAX_AMP * (1-cosf(gBatOrbitAngle * 3.14/180)) * 0.5f
        };
        gPositionBat.x = gPositionMario.x + 20.0f * cosf(gBatOrbitAngle*3.14/180);
        gPositionBat.y = gPositionMario.y + 20.0f * sinf(gBatOrbitAngle*3.14/180);
        
    }  
    else if (frames < 300)
    {   //kill spark screen for 100 frames
        gActiveBG = gKillSparkTexture;
    }
    else if (frames < 350)
    {   //sonic gets hit and travels the path of
        //a sqrt curve
        //multiplied by 5 to accentuate the curve
        gActiveBG = gBattlefieldTexture;
        gSonicAngle += 1080 * FPS / 50 * deltaTime;
        gPositionSonic.x += (SCREEN_WIDTH - SONIC_START.x + TEXTURE_SIZE / 2.0f) * (FPS / 50.0f) * deltaTime;
        gPositionSonic.y = SONIC_START.y - 5 * sqrtf(gPositionSonic.x - SONIC_START.x);
    }
    else if (frames == FRAME_LIMIT)
    {   //reset the scene once frame limit hit
        frames = 0;
        gMarioSpeedy = -4.0f * gMarioJumpHeight / t;
        gPositionMario = MARIO_START;
        gPositionBat = BAT_START;
        gBatOrbitAngle = 0.0f;
        gBatAngle = 45.0f;
        gPositionSonic = SONIC_START;
        gSonicAngle = 0.0f;
    }

}

void render()
{
    BeginDrawing();
    ClearBackground(ColorFromHex(BG_COLOUR));

    //setting background texture positions
    Rectangle textureAreaBG = {
        0.0f, 0.0f,

        static_cast<float>(gActiveBG.width),
        static_cast<float>(gActiveBG.height)
    };
    Rectangle destinationAreaBG = {
        0,
        0,
        SCREEN_WIDTH,
        SCREEN_HEIGHT
    };
    Vector2 objectOriginBG = {
        0,
        0
    };

    //setting mario texture position
    Rectangle textureAreaMario = {
        0.0f, 0.0f,

        static_cast<float>(gMario.width),
        static_cast<float>(gMario.height)
    };
    Rectangle destinationAreaMario = {
        gPositionMario.x, gPositionMario.y,

        TEXTURE_SIZE,
        TEXTURE_SIZE
    };
    Vector2 objectOriginMario = {
        TEXTURE_SIZE / 2.0f,
        TEXTURE_SIZE / 2.0f
    };

    //setting home run bat texture position
    Rectangle textureAreaBat = {
        0.0f, 0.0f,

        static_cast<float>(gBat.width),
        static_cast<float>(gBat.height)
    };
    Rectangle destinationAreaBat = {
        gPositionBat.x, gPositionBat.y,

        static_cast<float>(gBatScale.x),
        static_cast<float>(gBatScale.y)
    };
    Vector2 objectOriginBat  = {
        static_cast<float>(gBatScale.x) / 2.0f,
        static_cast<float>(gBatScale.y) / 2.0f
    };
    
    //setting sonic texture position
    Rectangle textureAreaSonic = {
        0.0f, 0.0f,

        static_cast<float>(gSonic.width),
        static_cast<float>(gSonic.height)
    };
    Rectangle destinationAreaSonic = {
        gPositionSonic.x, gPositionSonic.y,

        TEXTURE_SIZE,
        TEXTURE_SIZE
    };
    Vector2 objectOriginSonic = {
        TEXTURE_SIZE / 2.0f,
        TEXTURE_SIZE / 2.0f
    };
    //render all textures
    DrawTexturePro(
        gActiveBG,
        textureAreaBG,
        destinationAreaBG,
        objectOriginBG,
        0.0f,
        WHITE
    );

    DrawTexturePro(
        gMario,
        textureAreaMario,
        destinationAreaMario,
        objectOriginMario,
        0.0f,
        WHITE
    );

    DrawTexturePro(
        gBat,
        textureAreaBat,
        destinationAreaBat,
        objectOriginBat,
        gBatAngle,
        WHITE
    );

    DrawTexturePro(
        gSonic,
        textureAreaSonic,
        destinationAreaSonic,
        objectOriginSonic,
        gSonicAngle,
        WHITE
    );

    EndDrawing();
}

void shutdown()
{
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

    UnloadTexture(gBattlefieldTexture);
    UnloadTexture(gKillSparkTexture);
    UnloadTexture(gMario);
    UnloadTexture(gBat);
    UnloadTexture(gSonic);
    shutdown();

    return 0;
}