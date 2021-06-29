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

// Loading an image with STB_image
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Player.h"

struct GameState {
    Player* player;

};

GameState state;

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

GLuint LoadTexture(const char* filePath) {
    // Use stbi_load to load pixel data from an image file. On Computer Ram now
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);

    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }

    // Create a texture in OpenGl
    GLuint textureID;
    glGenTextures(1, &textureID);
    // Bind textureID to a target GL_TEXTURE_2D and let GL_TEXTURE_2D load data of "image"
    glBindTexture(GL_TEXTURE_2D, textureID);
    // This is what sends the image to the graphics card.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    // Texture Filtering" Minification and Magnification. Use 
    // Use GL_LINEAR or GL_NEAREST
    // MIN = Minifying, MAG = Magnifying
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    // free image data on Computer Ram since data is in GPU now. 
    stbi_image_free(image);
    return textureID;
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Textured!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    // ShaderProgram is loading shaders used by Video Card
    // Loading shaders that can handle textured polygons
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);

    // Don't need to set the color of player anymore
    // program.SetColor(1.0f, 0.0f, 0.0f, 1.0f);

    glUseProgram(program.programID);
    
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    // Enable blending (off by default)
    glEnable(GL_BLEND);
    // Good setting for transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    state.player = new Player();
    state.player->position = glm::vec3(0);
    state.player->movement = glm::vec3(0);
    state.player->speed = glm::vec3(1);
    state.player->textureID = LoadTexture("george_0.png");

    state.player->animeRight = new int[4]{ 3, 7, 11, 15 };
    state.player->animeLeft = new int[4]{ 1, 5, 9, 13 };
    state.player->animeUp = new int[4]{ 2, 6, 10, 14 };
    state.player->animeDown = new int[4]{ 0, 4, 8, 12 };

    state.player->animeIndices = state.player->animeRight;

    state.player->animeCols = 4;
    state.player->animeRows = 4;
    state.player->animeFrames = 4;
    state.player->animeIndex = 0;
    state.player->animeTime = 0;

}

void ProcessInput() {

    state.player->movement = glm::vec3(0, 0, 0);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            gameIsRunning = false;
        }
    }

    const Uint8* keys = SDL_GetKeyboardState(NULL);
    if (keys[SDL_SCANCODE_A]) {
        state.player->animeIndices = state.player->animeLeft;
        state.player->movement.x = -1.0f;
    }
    else if (keys[SDL_SCANCODE_D]) {
        state.player->animeIndices = state.player->animeRight;
        state.player->movement.x = 1.0f;
    }
    if (keys[SDL_SCANCODE_W]) {
        state.player->animeIndices = state.player->animeUp;
        state.player->movement.y = 1.0f;
    }
    else if (keys[SDL_SCANCODE_S]) {
        state.player->animeIndices = state.player->animeDown;
        state.player->movement.y = -1.0f;
    }

    if (glm::length(state.player->movement) > 1.0f) {
        state.player->movement = glm::normalize(state.player->movement);
    }
    
}

float lastTicks = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;

    state.player->Update(deltaTime);

}

void DrawSpriteFromTextureAtlas(ShaderProgram* program, GLuint textureID, int index)
{
    int cols = 4;
    int rows = 4;

    float u = (float)(index % cols) / (float)cols;
    float v = (float)(index / cols) / (float)rows;
    float width = 1.0f / (float)cols;
    float height = 1.0f / (float)rows;
    float texCoords[] = { u, v + height, u + width, v + height, u + width, v,
    u, v + height, u + width, v, u, v };
    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    glBindTexture(GL_TEXTURE_2D, textureID);
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    state.player->Render(&program);

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
