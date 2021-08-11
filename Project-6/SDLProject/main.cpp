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
#include "Game_Level.h"
#include "Weapon_Room.h"
#include "Game_Over.h"
#include "You_Win.h"
#include <string>

Mix_Music* music;
Mix_Chunk* game_over;

Scene* currentScene;
Scene* sceneList[5];

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
GLuint uiBarTextureID;

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    displayWindow = SDL_CreateWindow("The Legend of Zelda", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 704, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif
    
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    music = Mix_LoadMUS("Legend_of_Zelda/bgm.mp3");
    Mix_PlayMusic(music, -1);
    
    game_over = Mix_LoadWAV("Legend_of_Zelda/game_over.wav");
    
    
    glViewport(0, 0, 640, 480);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-8.0f, 8.0f, -5.5f, 7.0f, -1.0f, 1.0f);

    glUseProgram(program.programID);

    glClearColor(0.9882f, 0.847f, 0.6588f, 1.0f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    sceneList[0] = new Start_Menu();
    sceneList[1] = new Game_Level();
    sceneList[2] = new Weapon_Room();
    sceneList[3] = new Game_Over();
    sceneList[4] = new You_Win();
    
    SwitchToScene(sceneList[0], 3);

    fontTextureID = Util::LoadTexture("font.png");
    uiBarTextureID = Util::LoadTexture("Legend_of_Zelda/black.png");
    uiviewMatrix = glm::mat4(1.0f);
    uiprojectionMatrix = glm::ortho(-8.0f, 8.0f, -5.5f, 7.0f, -1.0f, 1.0f);
}

void ProcessInput(){
    currentScene->ProcessInput();
    gameIsRunning = currentScene->state.gameIsRunning;
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
        
        deltaTime -= FIXED_TIMESTEP;
        
        viewMatrix = currentScene->state.viewMatrix;
    }
    accumulator = deltaTime;
    

}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);

    currentScene->Render(&program);

    if (currentScene == sceneList[1] || currentScene == sceneList[2]){
        modelMatrix = glm::mat4(1.0f);
        program.SetProjectionMatrix(uiprojectionMatrix);
        program.SetViewMatrix(uiviewMatrix);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-8, 5.5f, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(16, 2, 1));
        program.SetModelMatrix(modelMatrix);
        Util::DrawUIBar(&program, uiBarTextureID);
        
        Util::DrawText(&program, fontTextureID, "Enemy Count: ", 0.75f, -0.35f, glm::vec3(-7.5f, 6.25f, 0));
        Util::DrawText(&program, fontTextureID, std::to_string(currentScene->state.enemiesAlive), 1.0f, -0.3f, glm::vec3(-2.5f, 6.25f, 0));
        
        Util::DrawText(&program, fontTextureID, "Lives:", 0.75f, -0.35f, glm::vec3(4.25f, 6.25f, 0));
        Util::DrawText(&program, fontTextureID, std::to_string(currentScene->state.player->lives), 1.0f, 0, glm::vec3(7.0f, 6.25f, 0));
    }
        
    SDL_GL_SwapWindow(displayWindow);
}



void Shutdown() {
    Mix_FreeMusic(music);
    
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
                // play bgm
            }
            else if (currentScene == sceneList[1] || currentScene == sceneList[2])
                lives = currentScene->state.player->lives;
            else
                lives = 0;
            if (currentScene->state.nextScene == 0)
                Mix_PlayMusic(music, -1);
            if (currentScene->state.nextScene == 4 || currentScene->state.nextScene == 3){ // win
                Mix_HaltMusic();
                Mix_PlayChannel(-1, game_over, 0);
            }
                
            if (currentScene == sceneList[2]){
                glm::vec3 start_position = glm::vec3(21, -14, 0);
                bool armed = currentScene->state.player->armed;
                bool visited = currentScene->state.player->visited;
                SwitchToScene(sceneList[currentScene->state.nextScene], lives);
                currentScene->state.player->position = start_position;
                currentScene->state.player->animIndices = currentScene->state.player->animDown;
                currentScene->state.player->armed = armed;
                currentScene->state.player->visited = visited;
            }
            else
                SwitchToScene(sceneList[currentScene->state.nextScene], lives);
        }
        Render();
    }
    
    Shutdown();
    return 0;
}
