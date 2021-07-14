#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Entity.h"

#define PLATFORM_COUNT 16
#define ENEMIES_COUNT 3

struct GameState {
    Entity *player;
    Entity* platform;
    Entity* enemies;
    Entity* smessage;
    Entity* fmessage;
};

GameState state;

SDL_Window* displayWindow;
bool gameIsRunning = true;

enum GameMode {GAME_LEVEL, GAME_OVER};
GameMode mode = GAME_LEVEL;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);
    
    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    stbi_image_free(image);
    return textureID;
}


void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Super Mario Bros", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);

    glUseProgram(program.programID);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    // Initialize Game Objects

    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(-4, -2, 0);
    state.player->movement = glm::vec3(0);
    state.player->speed = 3.0f;

    state.player->acceleration = glm::vec3(0, -9.81f, 0);
    state.player->jumpPower = 5.0f;

    state.player->width = 0.5f;
    state.player->height = 0.5f;

    state.player->textureID = LoadTexture("mario_sprite.png");

    state.player->animRight = new int[4]{ 4, 5, 6, 7 };
    state.player->animLeft = new int[4]{ 3, 2, 1, 0 };
    state.player->animJumpLeft = new int[4]{8, 8, 8, 8};
    state.player->animJumpRight = new int[4]{9, 9, 9, 9};
    

    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 4;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 4;
    state.player->animRows = 4;

    // Initilize Game Over Messages
    // Initilize Succuess Message
    GLuint fontTextureID = LoadTexture("font.png");

    state.smessage = new Entity();
    state.smessage->textureID = fontTextureID;
    state.smessage->entityType = TEXT;
    state.smessage->isActive = false;
    state.smessage->text = "You Win!";
    state.smessage->position = glm::vec3(-2.0f, 0.75f, 0);

    // Initilize Failure Message
    state.fmessage = new Entity();
    state.fmessage->textureID = fontTextureID;
    state.fmessage->entityType = TEXT;
    state.fmessage->isActive = false;
    state.fmessage->text = "You Lose";
    state.fmessage->position = glm::vec3(-2.0f, 0.75f, 0);
    
    // Initilize Platforms
    state.platform = new Entity[PLATFORM_COUNT];
    GLuint platformTextureID = LoadTexture("blocks.png");


    for (int i = 0; i < 11; i++) {
        state.platform[i].entityType = PLATFORM;
        state.platform[i].textureID = platformTextureID;
        state.platform[i].position = glm::vec3(-5 + i, -3.25f, 0);
    }
    
    for (int i = 11, j = 1; i < 14; i++, j--){
        state.platform[i].entityType = PLATFORM;
        state.platform[i].textureID = platformTextureID;
        state.platform[i].position = glm::vec3(-5.5 * j, -2.25f, 0);
    }
    
    for (int i = 14, j = 1; i < 16; i++, j-=2){
        state.platform[i].entityType = PLATFORM;
        state.platform[i].textureID = platformTextureID;
        state.platform[i].position = glm::vec3(-5.5 * j, -1.25f, 0);
    }
    
    for (int i = 0; i < PLATFORM_COUNT; i++) {
        state.platform[i].Update(0, NULL, NULL, 0, NULL, 0);
    }
    

    state.enemies = new Entity[ENEMIES_COUNT];
    GLuint enemyTextureID = LoadTexture("enemies_sprites.png");
    
    state.enemies[0].textureID = enemyTextureID;
    state.enemies[0].position = glm::vec3(-1.25, -2.25f, 0);
    state.enemies[0].entityType = ENEMY;
    state.enemies[0].aiType = GOOMBA;
    state.enemies[0].aiState = WALKING;
    state.enemies[0].velocity.x = -0.5f;
    
    state.enemies[0].animRight = new int[2] {1, 0};
    state.enemies[0].animLeft = new int[2] {0, 1};
    
    state.enemies[0].animIndices = state.enemies[0].animLeft;
    state.enemies[0].animFrames = 2;
    state.enemies[0].animTime = 0;
    state.enemies[0].animCols = 4;
    state.enemies[0].animRows = 4;
    
    state.enemies[0].width = 0.5;
    state.enemies[0].height = 0.5;
    
    state.enemies[0].acceleration = glm::vec3(0.0f, -9.81f, 0.0f);

    // Koopa Troopa
    state.enemies[1].textureID = enemyTextureID;
    state.enemies[1].position = glm::vec3(4, -2.25f, 0);
    state.enemies[1].entityType = ENEMY;
    state.enemies[1].aiType = KOOPA_TROOPA;
    state.enemies[1].aiState = IDLE;
    
    state.enemies[1].animRight = new int[2] {10, 11};
    state.enemies[1].animLeft = new int[2] {8, 9};
    
    state.enemies[1].animIndices = state.enemies[1].animLeft;
    state.enemies[1].animFrames = 2;
    state.enemies[1].animTime = 0;
    state.enemies[1].animCols = 4;
    state.enemies[1].animRows = 4;
    
    state.enemies[1].width = 0.5;
    state.enemies[1].height = 0.8;
    
    state.enemies[1].acceleration = glm::vec3(0.0f, -9.81f, 0.0f);
    
    // Koopa Paratroopa
    state.enemies[2].textureID = enemyTextureID;
    state.enemies[2].position = glm::vec3(2, 0, 0);
    state.enemies[2].entityType = ENEMY;
    state.enemies[2].aiType = KOOPA_PARATROOPA;
    state.enemies[2].aiState = FLYING;
    
    state.enemies[2].animUp = new int[2] {4, 5};
    state.enemies[2].animDown = new int[2] {4, 5};
    
    state.enemies[2].animIndices = state.enemies[2].animDown;
    state.enemies[2].animFrames = 2;
    state.enemies[2].animTime = 0;
    state.enemies[2].animCols = 4;
    state.enemies[2].animRows = 4;
    
    state.enemies[2].width = 0.5;
    state.enemies[2].height = 0.8;
    
}


void ProcessInputGameLevel() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                        // Move the player left
                        break;
                        
                    case SDLK_RIGHT:
                        // Move the player right
                        break;
                        
                    case SDLK_SPACE:
                        if (state.player->collidedBottom){
                            state.player->jump = true;
                            if (state.player->animIndices == state.player->animLeft)
                                state.player->animIndices = state.player->animJumpRight;
                            else if (state.player->animIndices == state.player->animRight)
                                state.player->animIndices = state.player->animJumpRight;
                        }
                        break;
                }
                break; // SDL_KEYDOWN
        }
    }
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT]) {
        state.player->velocity.x -= 0.1f;
        if (state.player->collidedBottom == false)
            state.player->animIndices = state.player->animJumpLeft;
        else
            state.player->animIndices = state.player->animLeft;
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        state.player->velocity.x += 0.1f;
        if (state.player->collidedBottom == false)
            state.player->animIndices = state.player->animJumpRight;
        else
            state.player->animIndices = state.player->animRight;
    }

}

void ProcessInputGameOver() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            gameIsRunning = false;
            break;
        }
    }
}


void ProcessInput() {
    switch (mode) {
        case GAME_LEVEL:
            ProcessInputGameLevel();
            break;
        case GAME_OVER:
            ProcessInputGameOver();
            break;
    }
}


#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;
void UpdateGameLevel() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    deltaTime += accumulator;
    if (deltaTime < FIXED_TIMESTEP) {
        accumulator = deltaTime;
        return;
    }
    while (deltaTime >= FIXED_TIMESTEP) {
        // Update. Notice it's FIXED_TIMESTEP. Not deltaTime
        state.player->Update(FIXED_TIMESTEP, NULL, state.platform, PLATFORM_COUNT, state.enemies, ENEMIES_COUNT);
        
        if (state.player->isActive == false) {
            mode = GAME_OVER;
            state.fmessage->isActive = true;
        }
        
        int activeEnemies = 0;
        for (int i = 0; i < ENEMIES_COUNT; i++) {
            state.enemies[i].Update(FIXED_TIMESTEP, state.player, state.platform, PLATFORM_COUNT, NULL, 0);
            if (state.enemies[i].isActive == true)
                activeEnemies++;
        }
        if (activeEnemies == 0){
            mode = GAME_OVER;
            state.smessage->isActive = true;
        }

        deltaTime -= FIXED_TIMESTEP;
    }
    accumulator = deltaTime;
}


void Update(){
    switch (mode) {
        case GAME_LEVEL:
            UpdateGameLevel();
            break;
        case GAME_OVER:
            break;
        default:
            break;
    }
}


void Render() {

    glClear(GL_COLOR_BUFFER_BIT);

    for (int i = 0; i < PLATFORM_COUNT; i++) {
        state.platform[i].Render(&program);
    }

    for (int i = 0; i < ENEMIES_COUNT; i++) {
        state.enemies[i].Render(&program);
    }
    
    state.player->Render(&program);
    
    state.smessage->Render(&program);
    state.fmessage->Render(&program);
    
    SDL_GL_SwapWindow(displayWindow);
}


void Shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();
    
    while (gameIsRunning) {
        ProcessInput();
        Update();
        Render();
    }
    
    Shutdown();
    return 0;
}
