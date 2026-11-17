#include "CS3113/Defeat.h"
#include "CS3113/Victory.h"

Camera2D gCamera = {};
Scene *gCurrentScene = nullptr;
std::map<SceneID, Scene*> gLevels = {};

// Global Constants
constexpr int SCREEN_WIDTH  = 1000,
              SCREEN_HEIGHT = 600,
              FPS           = 120;

constexpr char    BG_COLOUR[]      = "#C0897E";
constexpr Vector2 ORIGIN           = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
constexpr float FIXED_TIMESTEP     = 1.0f / 60.0f;

float leftBound = 0.0f,
      rightBound = 0.0f,
      bottomBound = 0.0f;

//h: 14 * 60
//v: 24 * 48



// Global Variables
AppStatus gAppStatus       = RUNNING;
float     gPreviousTicks   = 0.0f,
          gTimeAccumulator = 0.0f;

// Function Declarations
void switchToScene(Scene *scene);
void initialise();
void processInput();
void update();
void render();
void shutdown();

void switchToScene(Scene *scene)
{
    if (gCurrentScene) gCurrentScene->shutdown();
    gCurrentScene = scene;
    gCurrentScene->initialise();

    if (gCurrentScene->getState().soldier != nullptr){
        leftBound = gCurrentScene->getState().map->getLeftBoundary() + SCREEN_WIDTH / 2.0f,
        rightBound = gCurrentScene->getState().map->getRightBoundary() - SCREEN_WIDTH / 2.0f;
        bottomBound = gCurrentScene->getState().map->getBottomBoundary() - SCREEN_HEIGHT / 2.0f;
        gCamera.target = gCurrentScene->getState().soldier->getPosition();
    }
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Maps");
    InitAudioDevice();

    gLevels[MENU] = new Menu(ORIGIN, "#000000");
    gLevels[LEVEL_A] = new LevelA(ORIGIN, "#24382C");
    gLevels[LEVEL_B] = new LevelB(ORIGIN, "#E8A765");
    gLevels[LEVEL_C] = new LevelC(ORIGIN, "#928E88");
    gLevels[BOSS] = new Boss(ORIGIN, "#B80F0A");
    gLevels[VICTORY] = new Victory(ORIGIN, "#000000");
    gLevels[DEFEAT] = new Defeat(ORIGIN, "#000000");

    switchToScene(gLevels[MENU]);
    
    //camera                          // zero initialise
    gCamera.offset   = ORIGIN;                        // centre of screen
    gCamera.rotation = 0.0f;                          // no rotation
    gCamera.zoom     = 1.0f; 
    SetTargetFPS(FPS);
    gPreviousTicks = GetTime();
}

void processInput()
{
    gCurrentScene->processInput();
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
        gCurrentScene->update(FIXED_TIMESTEP);
        deltaTime -= FIXED_TIMESTEP;

        if (gCurrentScene->getState().soldier != nullptr){
            Vector2 currentPlayerPosition = gCurrentScene->getState().soldier->getPosition();
            panCamera(&gCamera, &currentPlayerPosition, leftBound, rightBound, bottomBound);
        }
    }

    gTimeAccumulator = deltaTime;

    /*if (gState.soldier->getPosition().y > END_GAME_THRESHOLD)
        gAppStatus = TERMINATED;*/
}

void render()
{
    BeginDrawing();
    if (gCurrentScene->getState().soldier != nullptr)
        BeginMode2D(gCamera);
    gCurrentScene->render();
    if (gCurrentScene->getState().soldier != nullptr) {
        EndMode2D();
        const char *lives = TextFormat("Lives: %i", gCurrentScene->getLives());
        DrawText(lives, 10.0f, 10.0f, 20, WHITE);
    }
    EndDrawing();
}

void shutdown()
{
    gCurrentScene->shutdown();
    for (std::pair<const SceneID, Scene *> &entry : gLevels)
        delete entry.second;

    gLevels.clear();

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

        if (gCurrentScene->getState().nextSceneID != NO_SCENE)
        {
            SceneID id = gCurrentScene->getState().nextSceneID;
            switchToScene(gLevels[id]);
        }

        render();
    }

    shutdown();

    return 0;
}
