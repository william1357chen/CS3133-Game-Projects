#include "Level1.h"
#include "Util.h"
#define LEVEL1_WIDTH 40
#define LEVEL1_HEIGHT 14

#define LEVEL1_ENEMY_COUNT 5

unsigned int level1_data[560] =
{
7, 0, 0, 36, 37, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 17, 18, 19,
7, 0, 0, 44, 45, 0, 0, 0, 0, 0, 0, 0, 16, 17, 18, 19, 0, 0, 0, 0, 0, 9, 10, 11, 0, 0, 0, 0, 36, 37, 0, 0, 0, 0, 0, 0, 24, 25, 26, 27,
7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 24, 25, 26, 27, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 44, 45, 0, 0, 0, 0, 0, 31, 0, 0, 0, 0,
7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 39, 0, 0, 0, 0,
7, 0, 0, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 47, 0, 0, 0, 0,
7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 55, 0, 0, 0, 0,
7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 55, 0, 0, 0, 0,
7, 0, 0, 0, 23, 0, 0, 6, 23, 6, 23, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 5, 0, 0, 0, 0, 0, 55, 0, 0, 0, 0,
7, 0, 0, 43, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 5, 0, 0, 0, 0, 12, 13, 0, 0, 0, 0, 0, 55, 0, 0, 0, 0,
7, 0, 50, 51, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 5, 0, 0, 33, 0, 12, 13, 0, 0, 0, 0, 12, 13, 0, 0, 0, 0, 0, 55, 0, 0, 0, 0,
7, 57, 58, 59, 60, 61, 0, 0, 0, 0, 0, 0, 28, 29, 30, 0, 12, 13, 0, 40, 41, 42, 12, 13, 0, 0, 0, 0, 12, 13, 0, 28, 29, 30, 0, 63, 0, 0, 0, 0,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7
};

void Level1::Initialize(int lives){
    state.nextScene = -1;
    GLuint mapTextureID = Util::LoadTexture("tilesheet.png");
    state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, level1_data, mapTextureID, 1.0f, 8, 8);
    // Initialize Game Objects

    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(1, -10, 0);
    state.player->startPosition = state.player->position;
    state.player->movement = glm::vec3(0);
    state.player->speed = 3.0f;
    state.player->lives = lives;

    state.player->acceleration = glm::vec3(0, -13.0f, 0);

    state.player->width = 0.90f;
    state.player->height = 0.90f;

    state.player->textureID = Util::LoadTexture("mario_sprites.png");

    state.player->animRight = new int[4]{ 7, 6, 4, 4 };
    state.player->animLeft = new int[4]{ 3, 2, 1, 0 };
    state.player->animJumpLeft = new int[4]{8, 8, 8, 8};
    state.player->animJumpRight = new int[4]{9, 9, 9, 9};
    

    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 4;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 4;
    state.player->animRows = 4;
    

    state.enemies = new Entity[LEVEL1_ENEMY_COUNT];
    GLuint enemyTextureID = Util::LoadTexture("enemy_sprites.png");
    
    for (int i = 0; i < 4; i++){
        state.enemies[i].textureID = enemyTextureID;
        if (i == 0)
            state.enemies[i].position = glm::vec3(14, -10, 0);
        else if (i == 1)
            state.enemies[i].position = glm::vec3(20, -10, 0);
        else if (i == 2 || i == 3)
            state.enemies[i].position = glm::vec3(22 + i * 1.5, -10, 0);
        state.enemies[i].startPosition = state.enemies[i].position;
        state.enemies[i].entityType = ENEMY;
        state.enemies[i].aiType = GOOMBA;
        state.enemies[i].aiState = WALKING;
        state.enemies[i].velocity.x = -0.5f;
        
        state.enemies[i].animRight = new int[2] {1, 0};
        state.enemies[i].animLeft = new int[2] {0, 1};
        
        state.enemies[i].animIndices = state.enemies[i].animLeft;
        state.enemies[i].animFrames = 2;
        state.enemies[i].animTime = 0;
        state.enemies[i].animCols = 4;
        state.enemies[i].animRows = 4;
        
        state.enemies[i].width = 0.55;
        state.enemies[i].height = 0.525;
        
        state.enemies[i].acceleration = glm::vec3(0.0f, -9.81f, 0.0f);
    }
    

    // Koopa Troopa
//    state.enemies[4].textureID = enemyTextureID;
//    state.enemies[4].position = glm::vec3(4, -2.25f, 0);
//    state.enemies[4].entityType = ENEMY;
//    state.enemies[4].aiType = KOOPA_TROOPA;
//    state.enemies[4].aiState = IDLE;
//
//    state.enemies[4].animRight = new int[2] {10, 11};
//    state.enemies[4].animLeft = new int[2] {8, 9};
//
//    state.enemies[4].animIndices = state.enemies[4].animLeft;
//    state.enemies[4].animFrames = 2;
//    state.enemies[4].animTime = 0;
//    state.enemies[4].animCols = 4;
//    state.enemies[4].animRows = 4;
//
//    state.enemies[4].width = 0.6;
//    state.enemies[4].height = 1.0f;
//
//    state.enemies[4].acceleration = glm::vec3(0.0f, -9.81f, 0.0f);
    
// Koopa Paratroopa
    state.enemies[4].textureID = enemyTextureID;
    state.enemies[4].position = glm::vec3(31.5f, -3.0f, 0);
    state.enemies[4].startPosition = state.enemies[4].position;
    state.enemies[4].entityType = ENEMY;
    state.enemies[4].aiType = KOOPA_PARATROOPA;
    state.enemies[4].aiState = FLYING;
    
    state.enemies[4].animUp = new int[2] {4, 5};
    state.enemies[4].animDown = new int[2] {4, 5};
    
    state.enemies[4].animIndices = state.enemies[4].animDown;
    state.enemies[4].animFrames = 2;
    state.enemies[4].animTime = 0;
    state.enemies[4].animCols = 4;
    state.enemies[4].animRows = 4;
    
    state.enemies[4].width = 0.6f;
    state.enemies[4].height = 1.0f;
}


void Level1::Update(float deltaTime){
    state.player->Update(deltaTime, NULL, state.enemies, LEVEL1_ENEMY_COUNT, state.map);
    if (state.player->complete == true) state.nextScene = 2;
    if (state.player->end == true) state.nextScene = 4;
    
    
    for (int i = 0; i < LEVEL1_ENEMY_COUNT; i++){
        state.enemies[i].Update(deltaTime, state.player, NULL, 0, state.map);
    }
    
}

void Level1::Render(ShaderProgram* program){
    state.map->Render(program);
    
    state.player->Render(program);
    
    for (int i = 0; i < LEVEL1_ENEMY_COUNT; i++){
        state.enemies[i].Render(program);
    }
}
