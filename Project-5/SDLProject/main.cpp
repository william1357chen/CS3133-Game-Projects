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


#include "Util.h"

#include "Entity.h"
#include "Map.h"
#include "Scene.h"
#include "Start_Menu.h"
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"


Scene* currentScene;
Scene* sceneList[4];

void SwitchToScene(Scene *scene) {
    currentScene = scene;
    currentScene->Initialize();
}

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

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

    sceneList[0] = new Start_Menu();
    sceneList[1] = new Level1();
    sceneList[2] = new Level2();
    sceneList[3] = new Level3();
    
    SwitchToScene(sceneList[1]);
    
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
                        if (currentScene->state.player->collidedBottom){
                            currentScene->state.player->jump = true;
                            if (currentScene->state.player->animIndices == currentScene->state.player->animLeft)
                                currentScene->state.player->animIndices = currentScene->state.player->animJumpLeft;
                            else if (currentScene->state.player->animIndices == currentScene->state.player->animRight)
                                currentScene->state.player->animIndices = currentScene->state.player->animJumpRight;
                        }
                        break;
                }
                break; // SDL_KEYDOWN
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
}

void ProcessInputStartMenu(){
    SDL_Event event;
    while (SDL_PollEvent(&event)){
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
            case SDLK_RETURN:
                currentScene->state.nextScene = 1;
            default:
                break;
        }
    }
}

void ProcessInput(){
    if (currentScene == sceneList[0])
        ProcessInputStartMenu();
    else
        ProcessInputGameLevel();
}


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
        viewMatrix = glm::mat4(1.0f);
        
        if (currentScene->state.player->position.x >= 5.5f)
            viewMatrix = glm::translate(viewMatrix,glm::vec3((-currentScene->state.player->position.x), 9.0f, 0));
        else
            viewMatrix = glm::translate(viewMatrix,glm::vec3(-5.5, 9.0f, 0));
        deltaTime -= FIXED_TIMESTEP;
    }
    accumulator = deltaTime;
    

}


void Render() {
    program.SetViewMatrix(viewMatrix);

    glClear(GL_COLOR_BUFFER_BIT);

    currentScene->Render(&program);
    
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
        if (currentScene->state.nextScene >= 0)
            SwitchToScene(sceneList[currentScene->state.nextScene]);
        Render();
    }
    
    Shutdown();
    return 0;
}
