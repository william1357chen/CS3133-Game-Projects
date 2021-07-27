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
#include <SDL_mixer.h>

#include "Util.h"

#include "Entity.h"
#include "Map.h"
#include "Scene.h"
#include "Start_Menu.h"
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"
#include "Game_Over.h"
#include "You_Win.h"
#include <string>

#define ENEMYCOUNT 5

Mix_Music* music;
Mix_Chunk* jumpingSound;
Mix_Chunk* restart;
Mix_Chunk* gameOver;
Mix_Chunk* stageClear;
Mix_Chunk* flagPole;
Mix_Chunk* kick;

Scene* currentScene;
Scene* sceneList[6];

void SwitchToScene(Scene *scene, int lives) {
    currentScene = scene;
    currentScene->Initialize(lives);
}

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;
glm::mat4 uiprojectionMatrix, uiviewMatrix;
GLuint fontTextureID;

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    displayWindow = SDL_CreateWindow("Super Mario Bros", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif
    
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    music = Mix_LoadMUS("background_final.mp3");
    Mix_PlayMusic(music, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 4);
    
    jumpingSound = Mix_LoadWAV("jumpsound.wav");
    Mix_VolumeChunk(jumpingSound, MIX_MAX_VOLUME / 6);
    
    restart = Mix_LoadWAV("restart.wav");
    Mix_VolumeChunk(restart, MIX_MAX_VOLUME);
    
    gameOver = Mix_LoadWAV("game_over.wav");
    Mix_VolumeChunk(gameOver, MIX_MAX_VOLUME / 2);
    
    stageClear = Mix_LoadWAV("stage_clear.wav");
    Mix_VolumeChunk(stageClear, MIX_MAX_VOLUME / 2);
    
    flagPole = Mix_LoadWAV("flagpole.wav");
    Mix_VolumeChunk(flagPole, MIX_MAX_VOLUME / 1.5);
    
    kick = Mix_LoadWAV("kick.wav");
    Mix_VolumeChunk(kick, MIX_MAX_VOLUME);
    
    glViewport(0, 0, 640, 480);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-10.0f, 10.0f, -7.0f, 7.0f, -1.0f, 1.0f);

    glUseProgram(program.programID);

    glClearColor(0.36471f, 0.58039f, 0.97f, 1.0f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    sceneList[0] = new Start_Menu();
    sceneList[1] = new Level1();
    sceneList[2] = new Level2();
    sceneList[3] = new Level3();
    sceneList[4] = new Game_Over();
    sceneList[5] = new You_Win();
    
    SwitchToScene(sceneList[0], 3);

    fontTextureID = Util::LoadTexture("font.png");
    uiviewMatrix = glm::mat4(1.0f);
    uiprojectionMatrix = glm::ortho(-10.0f, 10.0f, -7.0f, 7.0f, -1.0f, 1.0f);
}


void ProcessInputGameLevel() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
        }
    }
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT]) {
        currentScene->state.player->velocity.x -= 0.1f;
        if (currentScene->state.player->collidedBottom == false)
            currentScene->state.player->animIndices = currentScene->state.player->animJumpLeft;
        else
            currentScene->state.player->animIndices = currentScene->state.player->animLeft;
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        currentScene->state.player->velocity.x += 0.1f;
        if (currentScene->state.player->collidedBottom == false)
            currentScene->state.player->animIndices = currentScene->state.player->animJumpRight;
        else
            currentScene->state.player->animIndices = currentScene->state.player->animRight;
    }
    
    if (keys[SDL_SCANCODE_SPACE]){
        if (currentScene->state.player->collidedBottom){
            if (currentScene->state.player->animIndices == currentScene->state.player->animLeft)
                currentScene->state.player->animIndices = currentScene->state.player->animJumpLeft;
            else if (currentScene->state.player->animIndices == currentScene->state.player->animRight)
                currentScene->state.player->animIndices = currentScene->state.player->animJumpRight;
        }
        currentScene->state.player->spaceOn = true;
        
        if (currentScene->state.player->jump == false && currentScene->state.player->collidedBottom == true){
            currentScene->state.player->jump = true;
            currentScene->state.player->velocity.y += 4.0f;
            Mix_PlayChannel(-1, jumpingSound, 0);
        }
            
    }
    else
        currentScene->state.player->spaceOn = false;
}

void ProcessInputMenus(){
    SDL_Event event;
    while (SDL_PollEvent(&event)){
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_RETURN:
                        if (currentScene == sceneList[0])
                            currentScene->state.nextScene = 1;
                        else
                            currentScene->state.nextScene = 0;
                        break;
                    default:
                    break;
                }
        }
    }
}

void ProcessInput(){
    if (currentScene == sceneList[1] || currentScene == sceneList[2] || currentScene == sceneList[3]){
        ProcessInputGameLevel();
    }
    else
        ProcessInputMenus();
}
int Current_Lives;
int deadEnemies;

#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;
void Update() {

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
        currentScene->Update(FIXED_TIMESTEP);
        
        deltaTime -= FIXED_TIMESTEP;
        viewMatrix = glm::mat4(1.0f);
        if (!(currentScene == sceneList[1] || currentScene == sceneList[2] || currentScene == sceneList[3])) continue;
        
        if (Current_Lives > currentScene->state.player->lives && currentScene->state.player->lives != 0){
            Mix_PlayChannel(-1, restart, 0);
            Current_Lives = currentScene->state.player->lives;
        }
        int counter = 0;
        for (int i = 0; i < ENEMYCOUNT; i++){
            if (currentScene->state.enemies[i].isActive == false)
                counter++;
        }
        if (counter > deadEnemies){
            deadEnemies = counter;
            Mix_PlayChannel(-1, kick, 0);
        }
        
        if (currentScene->state.player->position.x <= 10.5f)
            viewMatrix = glm::translate(viewMatrix,glm::vec3(-10.5f, 5.0f, 0));
        else if (currentScene->state.player->position.x >= 29.4)
            viewMatrix = glm::translate(viewMatrix,glm::vec3(-29.4f, 5.0f, 0));
        else
            viewMatrix = glm::translate(viewMatrix,glm::vec3((-currentScene->state.player->position.x), 5.0f, 0));
    }
    accumulator = deltaTime;
    

}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);

    currentScene->Render(&program);

    if (currentScene == sceneList[1] || currentScene == sceneList[2] || currentScene == sceneList[3]){
        program.SetProjectionMatrix(uiprojectionMatrix);
        program.SetViewMatrix(uiviewMatrix);
        Util::DrawText(&program, fontTextureID, "Lives:", 1.0f, -0.25f, glm::vec3(4.5, 6, 0));
        Util::DrawText(&program, fontTextureID, std::to_string(currentScene->state.player->lives), 1.25f, 0, glm::vec3(9, 6, 0));
    }
        
    SDL_GL_SwapWindow(displayWindow);
}



void Shutdown() {
    Mix_FreeChunk(jumpingSound);
    Mix_FreeChunk(gameOver);
    Mix_FreeChunk(stageClear);
    Mix_FreeChunk(flagPole);
    Mix_FreeChunk(restart);
    Mix_FreeMusic(music);
    Mix_FreeChunk(kick);
    
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();
    
    while (gameIsRunning) {
        ProcessInput();
        Update();
        if (currentScene->state.nextScene >= 0){
            int lives;
            if (currentScene == sceneList[0]){
                lives = 3;
                Current_Lives = 3;
            }
            else if (currentScene == sceneList[1] || currentScene == sceneList[2] || currentScene == sceneList[3])
                lives = currentScene->state.player->lives;
            else
                lives = 0;
            if (currentScene->state.nextScene == 0)
                Mix_PlayMusic(music, -1);
            if (currentScene->state.nextScene == 4){
                Mix_HaltMusic();
                Mix_PlayChannel(-1, gameOver, 0);
            }
            else if (currentScene->state.nextScene == 2 || currentScene->state.nextScene == 3){
                Mix_PlayChannel(-1, flagPole, 0);
            }
            else if (currentScene->state.nextScene == 5){
                Mix_HaltMusic();
                Mix_PlayChannel(-1, stageClear, 0);
            }
            deadEnemies = 0;
            SwitchToScene(sceneList[currentScene->state.nextScene], lives);
        }
        Render();
    }
    
    Shutdown();
    return 0;
}
