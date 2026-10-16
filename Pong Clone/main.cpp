/**
* Author: Josch Wang
* Assignment: Pong Clone
* Date due: 10/16/2026
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#include "CS3113/cs3113.h"

struct Shell{
    Vector2 position;
    Vector2 movement;
    int speed = 100;
};
// Global Constants
constexpr int SCREEN_WIDTH  = 800,
              SCREEN_HEIGHT = 450,
              FPS           = 60,
              SPEED         = 200;


constexpr char    BG_COLOUR[]    = "#222222";
constexpr Vector2 ORIGIN             = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 },
                  SHELL_BASE_SIZE = { 20.0f, 20.0f },
                  MARIO_BASE_SIZE   = { 40.0f, 100.0f },
                  MARIO_INIT_POS  = { ORIGIN.x - 300.0f, ORIGIN.y},
                  LUIGI_BASE_SIZE   = { 40.0f, 100.0f },
                  LUIGI_INIT_POS = {ORIGIN.x + 300.0f, ORIGIN.y};



constexpr char MARIO_FP[] = "assets/Mario-Tennis.png";
constexpr char LUIGI_FP[]   = "assets/Luigi_Tennis.png";
constexpr char KOOPA_SHELL_FP[] = "assets/Koopa-Shell.png";

// Global Variables
AppStatus gAppStatus     = RUNNING;
float     gAngle         = 0.0f,
          gPreviousTicks = 0.0f;

Vector2 gMarioPosition = MARIO_INIT_POS,
        gMarioMovement = { 0.0f, 0.0f },
        gMarioScale    = MARIO_BASE_SIZE,

        gLuigiPosition = LUIGI_INIT_POS,
        gLuigiMovement = { 0.0f, 0.0f },
        gLuigiScale    = LUIGI_BASE_SIZE,

        gShellScale    = SHELL_BASE_SIZE;


Texture2D gShell;
Texture2D gMario;
Texture2D gLuigi;

Music gCoconutMall;
Sound gShellKick;

int numBalls = 1;
int prevNumBalls = 0;
Shell shells[3];
bool player2 = false;
bool gameOver = false;
int gMarioScore = 0;
int gLuigiScore = 0;

// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();
bool isColliding(const Vector2 *positionA, const Vector2 *scaleA, const Vector2 *positionB, const Vector2 *scaleB);

// Function Definitions

/**
 * @brief Checks for a square collision between 2 Rectangle objects.
 *
 * @param positionA The position of the first object
 * @param scaleA The scale of the first object
 * @param positionB The position of the second object
 * @param scaleB The scale of the second object
 * @return true if a collision is detected,
 * @return false if a collision is not detected
 */
bool isColliding(const Vector2 *positionA, const Vector2 *scaleA,
                 const Vector2 *positionB, const Vector2 *scaleB)
{
    float xDistance = fabs(positionA->x - positionB->x) -
                      ((scaleA->x + scaleB->x) / 2.0f);
    float yDistance = fabs(positionA->y - positionB->y) -
                      ((scaleA->y + scaleB->y) / 2.0f);

    return xDistance < 0.0f && yDistance < 0.0f;
}
//prevent anything from going out of bounds
bool hitWall(Vector2 *position, Vector2 *scale){
    if (position->y < scale->y / 2.0f){
        position->y = scale->y / 2.0f;
        return true;
    }
    else if (position->y > SCREEN_HEIGHT - scale->y / 2.0f){
        position->y = SCREEN_HEIGHT - scale->y / 2.0f;
        return true;
    }
    return false;
}
//reverse shell vertical movement when hitting wall
void wallBounce(Vector2 *position, Vector2 *scale, Vector2 *movement){
    if (hitWall(position, scale)) movement->y *= -1.0f;
}
//start shell at middle with slower speed
void serveShell(Shell *shell){
    float dir = GetRandomValue(0, 1) == 0 ? -1.0f : 1.0f;
    float shellAngle = GetRandomValue (15, 45);
    if (GetRandomValue(0, 1) == 1) shellAngle *= -1.0f;
    shell->position = {ORIGIN.x, GetRandomValue(15, 85) * SCREEN_HEIGHT / 100.0f};
    shell->movement = {dir * cosf(shellAngle * 3.14/180), sinf(shellAngle * 3.14/180)};
     
}
//find closest incoming shell (for ai logic)
int closestShell(){
    int candidate = -1;
    for (int i = 0; i < numBalls; ++i){
        if (shells[i].movement.x > 0.0f && (candidate == -1 ||
            shells[i].position.x > shells[candidate].position.x)){
            candidate = i;
        }
    }
    return candidate;
}
//handle shell and paddle collision logic
bool paddleCollision(const Vector2 *paddlePos, const Vector2 *paddleScale, Shell *shell, float face){
    if (!isColliding(paddlePos, paddleScale, &shell->position, &gShellScale)) return false;
    //play shell kick sound when colliding with paddle
    PlaySound(gShellKick);
    float facePos = paddlePos->x + face * paddleScale->x / 2.0f;
    //determine angle of shell after hitting paddle
    if ((shell->position.x - facePos) * face > 0.0f){
        if (shell->movement.x * face < 0.0f){
            float angle = 45.0f * (shell->position.y - paddlePos->y) / (paddleScale->y / 2.0f) * 3.14/180;
            shell->movement= {face * cosf(angle), sinf(angle)};
        }
    }
    //adjust shell movement if hitting edge of paddle
    else{
        shell->movement.x = -face * cosf(45.0f * 3.14/180);
        if (shell->position.y > paddlePos->y) shell->movement.y = sinf(45.0f * 3.14/180);
        else shell->movement.y = -sinf(45.0f * 3.14/180);
    }
    return true;
} 
void renderObject(const Texture2D *texture, const Vector2 *position,
                  const Vector2 *scale)
{
    // Whole texture (UV coordinates)
    Rectangle textureArea = {
        // top-left corner
        0.0f, 0.0f,

        // bottom-right corner (of texture)
        static_cast<float>(texture->width),
        static_cast<float>(texture->height)
    };

    // Destination rectangle – centred on gPosition
    Rectangle destinationArea = {
        position->x,
        position->y,
        static_cast<float>(scale->x),
        static_cast<float>(scale->y)
    };

    // Origin inside the source texture (centre of the texture)
    Vector2 originOffset = {
        static_cast<float>(scale->x) / 2.0f,
        static_cast<float>(scale->y) / 2.0f
    };

    // Render the texture on screen
    DrawTexturePro(
        *texture,
        textureArea, destinationArea, originOffset,
        gAngle, WHITE
    );
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT,
        "Super Mario Pong");

    InitAudioDevice();
    gCoconutMall = LoadMusicStream("assets/Coconut_Mall.mp3");
    PlayMusicStream(gCoconutMall);
    gShellKick = LoadSound("assets/Kick_Shell.mp3");

    gMario = LoadTexture(MARIO_FP);
    gLuigi = LoadTexture(LUIGI_FP);
    gShell = LoadTexture(KOOPA_SHELL_FP);
    SetTargetFPS(FPS);

    serveShell(&shells[0]);
}

void processInput()
{
    gMarioMovement = {0.0f, 0.0f};
    gLuigiMovement = {0.0f, 0.0f};
    //w and s keys for player 1 movement
    if (IsKeyDown(KEY_W)) gMarioMovement.y =  -1.0f;
    else if (IsKeyDown(KEY_S)) gMarioMovement.y = 1.0f;
    
    //t key to turn player 2 on/off
    if (IsKeyPressed(KEY_T)) player2 = !player2;
    if (player2){
        //arrow keys for player 2 movement
        if (IsKeyDown(KEY_UP)) gLuigiMovement.y = -1.0f;
        else if (IsKeyDown(KEY_DOWN)) gLuigiMovement.y =  1.0f;
    }
    //while game is going, number key (1-3) corresponds to number of active shells
    if (!gameOver){
        if (IsKeyPressed(KEY_ONE) && numBalls != 1) {
            numBalls = 1;
            shells[0].speed = 100;
            serveShell(&shells[0]);
        }
        if (IsKeyPressed(KEY_TWO) && numBalls != 2) {
            numBalls = 2;
            for (int i = 0; i < numBalls; ++i){
                shells[i].speed = 100;
                serveShell(&shells[i]);
            }
        }
        if (IsKeyPressed(KEY_THREE) && numBalls != 3) {
            numBalls = 3;
            for (int i = 0; i < numBalls; ++i){
                shells[i].speed = 100;
                serveShell(&shells[i]);
            }
        }
    }
    //restart game, only after someone wins
    else if (IsKeyPressed(KEY_R)){
        gMarioScore = 0;
        gLuigiScore = 0;
        gameOver = false;
        numBalls = prevNumBalls;
        for (int i = 0; i < numBalls; ++i){
            shells[i].speed = 100;
            serveShell(&shells[i]);
        }
    }
    // to close the game
    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
}

void update()
{
    // delta time
    float ticks = static_cast<float>(GetTime());
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks  = ticks;
    
    UpdateMusicStream(gCoconutMall);
    //same numBalls to start with same amount next game
    if (gameOver && numBalls != 0) {
        prevNumBalls = numBalls;
        numBalls = 0;
    }
    //ai movement depending on closest incoming shell
    if (!player2){
        int target = closestShell();
        if (target != -1){
            if (shells[target].position.y > gLuigiPosition.y + gLuigiScale.y / 4.0f) gLuigiMovement.y = 1.0f;
            else if (shells[target].position.y < gLuigiPosition.y - gLuigiScale.y / 4.0f) gLuigiMovement.y = -1.0f;
            else gLuigiMovement.y = 0.0f;
        }
        else gLuigiMovement.y = 0.0f;
    }
    //control movement and prevent paddles from going out of bounds
    gMarioPosition.y = gMarioPosition.y + gMarioMovement.y * SPEED * deltaTime;
    gLuigiPosition.y = gLuigiPosition.y + gLuigiMovement.y * SPEED * deltaTime;
    hitWall(&gMarioPosition, &gMarioScale);
    hitWall(&gLuigiPosition, &gLuigiScale);

    //handles shell logic for each active shell
    for (int i = 0; i < numBalls; ++i){
        //speed up ball after first hit
        if (paddleCollision(&gMarioPosition, &gMarioScale, &shells[i], 1.0f) ||
            paddleCollision(&gLuigiPosition, &gLuigiScale, &shells[i], -1.0f)){
            shells[i].speed = 300;
        }
        //adjust shell movement and have it bounce off the wall
        shells[i].position.x = shells[i].position.x + shells[i].movement.x * shells[i].speed * deltaTime;
        shells[i].position.y = shells[i].position.y + shells[i].movement.y * shells[i].speed * deltaTime;
        wallBounce(&shells[i].position, &gShellScale, &shells[i].movement);
        //reset shell and adjust score if one goes past a paddle
        if (shells[i].position.x < -gShellScale.x / 2.0f|| shells[i].position.x > SCREEN_WIDTH + gShellScale.x / 2.0f) {
            if (shells[i].position.x < 0.0f) gLuigiScore++;
            else gMarioScore++;
            shells[i].speed = 100;
            serveShell(&shells[i]);
        }
    }
}

void render()
{
    BeginDrawing();
    ClearBackground(ColorFromHex(BG_COLOUR));
    //render text for scores
    const char* marioScore = TextFormat("%i", gMarioScore);
    const char* luigiScore = TextFormat("%i", gLuigiScore);
    DrawText(marioScore, SCREEN_WIDTH / 4 - MeasureText(marioScore, 60) / 2, SCREEN_HEIGHT / 10, 60, WHITE);
    DrawText(luigiScore, 3 * SCREEN_WIDTH / 4 - MeasureText(luigiScore, 60) / 2, SCREEN_HEIGHT / 10, 60, WHITE);

    //display winner when someone reaches 11 points
    const char* winMessage = nullptr;
    if (gMarioScore >= 11){
        winMessage = "Player 1 Wins!";
        gameOver = true;
    }
    else if (gLuigiScore >= 11){
        winMessage = "Player 2 Wins!";
        gameOver = true;
    }
    if (winMessage != nullptr){
        DrawText(winMessage, SCREEN_WIDTH / 2 - MeasureText(winMessage, 20) / 2, SCREEN_HEIGHT / 2, 20, WHITE);
    }
    // render mario
    renderObject(&gMario, &gMarioPosition, &gMarioScale);

    // render luigi
    renderObject(&gLuigi, &gLuigiPosition, &gLuigiScale);

    //render all active shells
    for (int i = 0; i < numBalls; i++){
        renderObject(&gShell, &shells[i].position, &gShellScale);
    }
    EndDrawing();
}

void shutdown()
{
    UnloadTexture(gMario);
    UnloadTexture(gLuigi);
    UnloadTexture(gShell);
    UnloadMusicStream(gCoconutMall);
    UnloadSound(gShellKick);
    CloseAudioDevice();
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