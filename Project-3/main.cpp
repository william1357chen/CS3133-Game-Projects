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

#define WALL_COUNT 26
#define LEFT_WALL_COUNT 8
#define RIGHT_WALL_COUNT 8
#define BOTTOM_WALL_COUNT 6
#define RANDOM_WALL_COUNT 4
#define PLATFORM_COUNT 2



struct GameState {
    Entity* player;
    Entity* wall;
    Entity* platform;
    Entity* smessage;
    Entity* fmessage;
};

GameState state;

enum GameMode {GAME_LEVEL, GAME_OVER};
GameMode mode = GAME_LEVEL;

SDL_Window* displayWindow;
bool gameIsRunning = true;

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
    displayWindow = SDL_CreateWindow("Lunar Lander", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
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
    
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
   
    // Initialize Game Objects
    
    // Initialize Player
    state.player = new Entity();
    state.player->entityType = Player;
    state.player->position = glm::vec3(0, 2.5f, 0);
    state.player->acceleration = glm::vec3(0, -0.1f, 0);

    state.player->textureID = LoadTexture("player.png");

    // Initilize Succuess Message
    GLuint fontTextureID = LoadTexture("font.png");

    state.smessage = new Entity();
    state.smessage->textureID = fontTextureID;
    state.smessage->entityType = Text;
    state.smessage->isActive = false;
    state.smessage->text = "Mission Successful";
    state.smessage->position = glm::vec3(-4.25f, 0.0f, 0);

    // Initilize Failure Message
    state.fmessage = new Entity();
    state.fmessage->textureID = fontTextureID;
    state.fmessage->entityType = Text;
    state.fmessage->isActive = false;
    state.fmessage->text = "Mission Failed";
    state.fmessage->position = glm::vec3(-3.25f, 0.0f, 0);

    // Initilize Walls
    state.wall = new Entity[WALL_COUNT];
    GLuint wallTextureID = LoadTexture("wall.png");

    float x, y;
    int i;
    // Initilize Left Walls
    for (i = 0, y = -3.25f; i < 8; i++, y++) {
        state.wall[i].textureID = wallTextureID;
        state.wall[i].entityType = Wall;
        state.wall[i].position = glm::vec3(-4.50f, y, 0);
    }

    // Initilize Right Walls
    for (i = 8, y = -3.25f; i < 16; i++, y++) {
        state.wall[i].textureID = wallTextureID;
        state.wall[i].entityType = Wall;
        state.wall[i].position = glm::vec3(4.50f, y, 0);
    }

    // Initilize Bottom Walls
    for (i = 16, x = -3.50f; i < 22; i++, x += 1.0f) {
        state.wall[i].textureID = wallTextureID;
        state.wall[i].entityType = Wall;
        state.wall[i].position = glm::vec3(x, -3.25f, 0);
    }

    // Initilize Random Walls
    for (i = 22, x = -3.50f; i < 24; i++, x++) {
        state.wall[i].textureID = wallTextureID;
        state.wall[i].entityType = Wall;
        state.wall[i].position = glm::vec3(x, 2.25f, 0);
    }

    for (i = 24, x = -4.50f + 1.0f * BOTTOM_WALL_COUNT; i < 26; i++, x++) {
        state.wall[i].textureID = wallTextureID;
        state.wall[i].entityType = Wall;
        state.wall[i].position = glm::vec3(x, 0, 0);
    }


    for (i = 0; i < WALL_COUNT; i++) {
        state.wall[i].Update(0);
    }

    // Initilize Platforms
    state.platform = new Entity[PLATFORM_COUNT];
    GLuint platformTextureID = LoadTexture("platform.png");
    for (i = 0, x = -3.50f + 1.0f * BOTTOM_WALL_COUNT; i < PLATFORM_COUNT; i++, x++) {
        state.platform[i].textureID = platformTextureID;
        state.platform[i].entityType = Platform;
        state.platform[i].position = glm::vec3(x, -3.25f, 0);
        state.platform[i].Update(0);
    }
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
    const Uint8* keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT]) {
        state.player->acceleration.x += -0.01f;
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        state.player->acceleration.x += 0.01f;
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
float accumulator = 0.0f;

void UpdateGameLevel(float deltaTime) {
    deltaTime += accumulator;
    if (deltaTime < FIXED_TIMESTEP) {
        accumulator = deltaTime;
        return;
    }
    while (deltaTime >= FIXED_TIMESTEP) {
        // Update. Notice it's FIXED_TIMESTEP. Not deltaTime
        state.player->Update(FIXED_TIMESTEP, state.wall, WALL_COUNT, state.platform, PLATFORM_COUNT);

        for (int i = 0; i < WALL_COUNT; i++) {
            state.wall[i].Update(FIXED_TIMESTEP);
        }
        deltaTime -= FIXED_TIMESTEP;
    }
    accumulator = deltaTime;
}


float lastTicks = 0;
void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks; 
    
    switch (mode) {
    case GAME_LEVEL:
        UpdateGameLevel(deltaTime);
        if (state.player->isActive == false) {
            mode = GAME_OVER;
            if (state.player->result == true)
                state.smessage->isActive = true;
            else
                state.fmessage->isActive = true;
        }

        break;
    case GAME_OVER:
        break;
    }
}

void RenderGameLevel() {

    for (int i = 0; i < WALL_COUNT; i++) {
        state.wall[i].Render(&program);
    }

    for (int i = 0; i < PLATFORM_COUNT; i++) {
        state.platform[i].Render(&program);
    }

    state.player->Render(&program);
}

void RenderGameOver() {
    RenderGameLevel();
    
    state.smessage->Render(&program);
    state.fmessage->Render(&program);

}




void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    switch (mode) {
    case GAME_LEVEL:
        RenderGameLevel();
        break;
    case GAME_OVER:
        RenderGameOver();
        break;
    }

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
