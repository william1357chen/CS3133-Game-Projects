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

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

#include <iostream>

class Illuminati {
public:
    Illuminati() : modelMatrix(glm::mat4(1.0f)), startingMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.75f, 0.0f))) {
        textureID = 0;
        rotation = 0.0f;
        scale_xy = 4.5f;
        scale_up = false;
    }

    void rotate(float deltaTime, float angle) {
        if (rotation >= 360.0f)
            rotation = 0.0f;
        rotation += angle * deltaTime;
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
    }

    void scale(float deltaTime, float scale_change_xy) {
        if (scale_xy >= 5.0f)
            scale_up = false;
        else if (scale_xy <= 0.5f)
            scale_up = true;

        if (scale_up == false)
            scale_xy -= scale_change_xy * deltaTime;
        else
            scale_xy += scale_change_xy * deltaTime;

        modelMatrix = glm::scale(modelMatrix, glm::vec3(scale_xy, scale_xy, 1.0f));
    }

    void update(float deltaTime, float scale_change_xy, float angle) {
        modelMatrix = startingMatrix;

        this->scale(deltaTime, scale_change_xy);
        this->rotate(deltaTime, angle);
    }

    const glm::mat4& getMatrix() const {
        return modelMatrix;
    }
    GLuint getTextureID() const {
        return textureID;
    }
    void setTextureID(GLuint textureID) {
        this->textureID = textureID;
    }
private:
    GLuint textureID;
    glm::mat4 startingMatrix;
    glm::mat4 modelMatrix;
    float rotation;
    float scale_xy;
    bool scale_up;
};

class Hand {
public:
    Hand() : modelMatrix(glm::mat4(1.0f)), startingMatrix(glm::mat4(1.0f)) {
        textureID = 0;
        translate_x = 0.0f;
        move_right = true;
    }

    void update(float deltaTime, float x) {
        modelMatrix = startingMatrix;
        this->translate(deltaTime, x);
    }
    void translate(float deltaTime, float x) {
        if (translate_x >= 1.5f)
            move_right = false;
        if (translate_x <= -1.5f)
            move_right = true;

        if (move_right == false)
            translate_x -= x * deltaTime;
        else
            translate_x += x * deltaTime;
        modelMatrix = glm::translate(modelMatrix, glm::vec3(translate_x, 0.0f, 0.0f));
    }

    void set_starting_position(float x, float y) {
        startingMatrix = glm::translate(modelMatrix, glm::vec3(x, y, 0.0f));
    }
    const glm::mat4& getMatrix() const {
        return modelMatrix;
    }
    GLuint getTextureID() const {
        return textureID;
    }
    void setTextureID(GLuint textureID) {
        this->textureID = textureID;
    }
private:
    GLuint textureID;
    glm::mat4 startingMatrix;
    glm::mat4 modelMatrix;
    float translate_x;
    bool move_right;
};

Illuminati triangle;

// Not sure why vectors don't work so I use arrays
#define numHands 5
Hand hands[numHands];


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

void initializeTriangle() {
    GLuint triangleTextureID = LoadTexture("illuminati.png");
    triangle.setTextureID(triangleTextureID);

}

void initializeHands() {
    float x = -3.0f;
    float y = -3.0f;

    for (size_t i = 0; i < numHands; i++) {
        GLuint handTextureID = LoadTexture("hands.png");
        hands[i].setTextureID(handTextureID);
        hands[i].set_starting_position(x, y);
        x += 1.5f;
    }
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Illuminati", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
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

    glEnable(GL_BLEND);
    // Good setting for transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glUseProgram(program.programID);
    
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    initializeTriangle();
    initializeHands();

}

void ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            gameIsRunning = false;
        }
    }
}

float lastTicks = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;

    triangle.update(deltaTime, 1.0f, 45.0f);

    for (size_t i = 0; i < numHands; i++) {
        hands[i].update(deltaTime, 1.0f);
    }
}

void drawTriangle() {
    program.SetModelMatrix(triangle.getMatrix());
    glBindTexture(GL_TEXTURE_2D, triangle.getTextureID());
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void drawHands() {
    for (size_t i = 0; i < numHands; i++) {
        program.SetModelMatrix(hands[i].getMatrix());
        glBindTexture(GL_TEXTURE_2D, hands[i].getTextureID());
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);
    
    drawTriangle();
    drawHands();

    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);

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
