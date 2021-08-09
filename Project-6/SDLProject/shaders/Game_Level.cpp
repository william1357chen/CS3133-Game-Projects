#include "Game_Level.h"
#include "Util.h"
#define GAME_LEVEL_WIDTH 48
#define GAME_LEVEL_HEIGHT 22

#define LEVEL1_ENEMY_COUNT 0


void Game_Level::Initialize(int lives){
    state.nextScene = -1;
    
    unsigned int* map_data = Util::LoadMapArray("map_data.txt", GAME_LEVEL_WIDTH, GAME_LEVEL_HEIGHT);
    GLuint mapTextureID = Util::LoadTexture("Legend_of_Zelda/tilesheet.png");
    state.map = new Map(GAME_LEVEL_WIDTH, GAME_LEVEL_HEIGHT, map_data, mapTextureID, 1.0f, 5, 3);
    // Initialize Game Objects

    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(0, 0, 0);
    state.player->startPosition = state.player->position;
    state.player->lives = lives;

    state.player->width = 0.8f;
    state.player->height = 1.0f;

    state.player->textureID = Util::LoadTexture("Legend_of_Zelda/link.png");

    state.player->animRight = new int[2]{ 2, 6};
    state.player->animLeft = new int[2]{ 3, 7};
    state.player->animUp = new int[2]{ 1, 5 };
    state.player->animDown = new int[2]{ 0, 4 };
    
    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 2;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 4;
    state.player->animRows = 5;
}

void Game_Level::ProcessInput() {
    state.player->velocity = glm::vec3(0);
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                state.gameIsRunning = false;
                break;
        }
    }
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_A]) {
        state.player->animIndices = state.player->animLeft;
        state.player->velocity.x = -5.0f;
    }
    else if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D]) {
        state.player->animIndices = state.player->animRight;
          state.player->velocity.x = 5.0f;
    }
    else if (keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W]) {
        state.player->animIndices = state.player->animUp;
        state.player->velocity.y = 5.0f;
    }
    else if (keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S]) {
        state.player->animIndices = state.player->animDown;
        state.player->velocity.y = -5.0f;
    }

}


void Game_Level::Update(float deltaTime){
    state.player->Update(deltaTime, NULL, state.enemies, LEVEL1_ENEMY_COUNT, state.map);
    if (state.player->complete == true) state.nextScene = 2;
    if (state.player->end == true) state.nextScene = 4;
    
    
    for (int i = 0; i < LEVEL1_ENEMY_COUNT; i++){
        state.enemies[i].Update(deltaTime, state.player, NULL, 0, state.map);
    }
    

    state.viewMatrix = glm::mat4(1.0f);
//    if (state.player->position.y <= -4.5f){
//        state.viewMatrix = glm::translate(state.viewMatrix, glm::vec3(0, 4.5f, 0));
//    }
//    else
        state.viewMatrix = glm::translate(state.viewMatrix, glm::vec3(0, -state.player->position.y, 0));
    state.viewMatrix = glm::translate(state.viewMatrix, glm::vec3(-state.player->position.x, 0, 0));
    
}

void Game_Level::Render(ShaderProgram* program){
    state.map->Render(program);
    
    state.player->Render(program);
    
    for (int i = 0; i < LEVEL1_ENEMY_COUNT; i++){
        state.enemies[i].Render(program);
    }
}
