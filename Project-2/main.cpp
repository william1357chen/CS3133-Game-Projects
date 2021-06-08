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


SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

const float ortho_width = 5.0f;
const float ortho_height = 3.75f;

struct border {
    // origin of border
    float x;
    float y;
    // width of border
    float width;
    // height of border is always 0
    float height;
};

// upper border has center at (0, 5.0) with width of 7.5 and height of 0
const border upper_border = { 0, ortho_height, 2.0f * ortho_width, 0 };
// bottom border has center at (0, -5.0) with width of 7.5 and height of 0
const border bottom_border = { 0, -ortho_height, 2.0f * ortho_width, 0 };
// left border has center at (-3.75, 0) with width of 0 and height of 10.0 
const border left_boarder = { -ortho_width, 0, 0, 2.0f * ortho_height };
// right border has center at (3.75, 0) with width of 0 and height of 10.0 
const border right_border = { ortho_width, 0, 0, 2.0f * ortho_height };


struct Player {
    void moveUp() {
        if (collisionCheck(upper_border)) {
            stopMove();
        }
        else{
            movement.y = 1.0f;
        }
    }
    void moveDown() {
        if (collisionCheck(bottom_border)) {
            stopMove();
        }
        else {
            movement.y = -1.0f;
        }
    }
    void stopMove() {
        movement.y = 0.0f;
    }

    // box-box collision detection with top and bottom border
    // return true if they are colliding
    bool collisionCheck(const border& border){
        float xdist = fabs(border.x - position.x) - ((border.width + scale.x) / 2.0f);
        float ydist = fabs(border.y - position.y) - ((border.height + scale.y) / 2.0f);
        return xdist < 0 && ydist < 0;
    }

    void move() {
        const Uint8* keys = SDL_GetKeyboardState(NULL);
        if (keys[upKey]) {
            moveUp();
        }
        else if (keys[downKey]) {
            moveDown();
        }
        else {
            stopMove();
        }
    }

    void update(float deltaTime) {
        matrix = glm::mat4(1.0f);

        // First translate to the correct position
        position += movement * speed * deltaTime;
        matrix = glm::translate(matrix, position);

        // Then scale down the player
        matrix = glm::scale(matrix, scale);
    }
    // Keep track of player position
    glm::vec3 position;
    // Keep track of player movement direction
    glm::vec3 movement;
    // Keep track of player movement speed
    glm::vec3 speed;
    // Keep track of the size of player
    glm::vec3 scale;
    // player's matrix
    glm::mat4 matrix;
    int upKey;
    int downKey;
};

Player player1;
Player player2;

struct Ball {

    void update(float deltaTime) {
        matrix = glm::mat4(1.0f);

        // First translate to the correct position
        position += movement * speed * deltaTime;
        matrix = glm::translate(matrix, position);

        // Then scale down the player
        matrix = glm::scale(matrix, scale);
    }
    // box-box collision detection with top, bottom, left, and right border
    // return true if they are colliding
    bool collisionCheck(const border& border) {
        float xdist = fabs(border.x - position.x) - ((border.width + scale.x) / 2.0f);
        float ydist = fabs(border.y - position.y) - ((border.height + scale.y) / 2.0f);
        return xdist < 0 && ydist < 0;
    }
    // box-box collision detection with players
    // return true if they are colliding
    bool collisionCheck(const Player& player) {
        float xdist = fabs(player.position.x - position.x) - ((player.scale.x + scale.x) / 2.0f);
        float ydist = fabs(player.position.y - position.y) - ((player.scale.y + scale.y) / 2.0f);
        return xdist < 0 && ydist < 0;
    }

    void move() {
        if (collisionCheck(upper_border)) {
            movement.y = -1.0f;
        }
        else if (collisionCheck(bottom_border)) {
            movement.y = 1.0f;
        }
        else if (collisionCheck(left_boarder) || collisionCheck(right_border)) {
            movement.x = 0.0f;
            movement.y = 0.0f;
            gameIsRunning = false;
        }
        else if (collisionCheck(player1)) {
            movement.x = 1.0f;
        }
        else if (collisionCheck(player2)) {
            movement.x = -1.0f;
        }
    }
    // Keep track of ball position
    glm::vec3 position;
    // Keep track of ball movement direction
    glm::vec3 movement;
    // Keep track of ball movement speed
    glm::vec3 speed;
    // Keep track of the size of ball
    glm::vec3 scale;
    // ball's matrix
    glm::mat4 matrix;

};

Ball gameBall;

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex.glsl", "shaders/fragment.glsl");
    
    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-ortho_width, ortho_width, -ortho_height, ortho_height, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    program.SetColor(1.0f, 1.0f, 1.0f, 1.0f);
    
    // Initialize players. The order is player_position, player_movement, player_speed, player_scale, player_matrix, upKey, downKey
    player1 = { glm::vec3(-4.75f, 0.0f, 0.0f), glm::vec3(0,0,0), glm::vec3(0, 5.0f, 0.0f), glm::vec3(0.5f, 2.0f, 1.0f), glm::mat4(1.0f), SDL_SCANCODE_W, SDL_SCANCODE_S };
    player2 = { glm::vec3(4.75f, 0.0f, 0.0f), glm::vec3(0,0,0), glm::vec3(0, 5.0f, 0.0f), glm::vec3(0.5f, 2.0f, 1.0f), glm::mat4(1.0f), SDL_SCANCODE_UP, SDL_SCANCODE_DOWN };

    // Initialize gameBall. The order is ball position, ball movement, ball speed, ball scale, and ball matrix
    gameBall = { glm::vec3(0,0,0), glm::vec3(0,0,0), glm::vec3(5.0f, 5.0f, 0.0f), glm::vec3(0.5f, 0.5f, 1.0f), glm::mat4(1.0f) };

    glUseProgram(program.programID);
    
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

}

void ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            gameIsRunning = false;
        }
        // Space starts the game
        else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
            gameBall.movement.x = 1.0f;
            gameBall.movement.y = 1.0f;
        }
    }
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    // players movement
    player1.move();
    player2.move();

    gameBall.move();
}

float lastTicks = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;

    player1.update(deltaTime);
    player2.update(deltaTime);
    gameBall.update(deltaTime);
}

void drawPlayer(const Player& player) {
    program.SetModelMatrix(player.matrix);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void drawBall() {
    program.SetModelMatrix(gameBall.matrix);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };

    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    drawPlayer(player1);
    drawPlayer(player2);
    drawBall();

    glDisableVertexAttribArray(program.positionAttribute);

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
