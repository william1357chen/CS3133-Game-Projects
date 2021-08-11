#pragma once
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


struct GameState {
    Map* map = NULL;
    Entity* player;
    Entity* enemies;
    int enemiesAlive;
    Entity* message;
    Mix_Chunk** soundEffects;
    glm::mat4 viewMatrix = glm::mat4(1.0f);
    bool gameIsRunning = true;
    int nextScene;

};

class Scene {
public:
    GameState state;
    virtual void Initialize(int lives) = 0;
    virtual void ProcessInput() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void Render(ShaderProgram *program) = 0;
};
