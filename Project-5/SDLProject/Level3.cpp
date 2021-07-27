#include "Level3.h"
#include "Util.h"
#define LEVEL3_WIDTH 40
#define LEVEL3_HEIGHT 14

#define LEVEL3_ENEMY_COUNT 5

unsigned int level3_data[560] =
{
7, 0, 0, 36, 37, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 62, 16, 17, 18, 19,
7, 0, 0, 44, 45, 0, 0, 0, 0, 0, 0, 0, 16, 17, 18, 19, 0, 0, 0, 0, 0, 9, 10, 11, 0, 0, 0, 0, 36, 37, 0, 0, 0, 0, 0, 62, 24, 25, 26, 27,
7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 24, 25, 26, 27, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 44, 45, 0, 0, 0, 0, 0, 31, 0, 0, 0, 0,
7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 39, 0, 0, 0, 0,
7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 47, 0, 0, 0, 0,
7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 55, 0, 0, 0, 0,
7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 23, 6, 0, 0, 0, 0, 0, 0, 55, 0, 0, 0, 0,
7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 63, 0, 0, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 55, 0, 0, 0, 0,
7, 0, 0, 0, 0, 0, 63, 0, 0, 63, 0, 0, 0, 0, 0, 0, 0, 63, 63, 0, 0, 0, 63, 63, 0, 0, 43, 0, 0, 0, 0, 0, 0, 0, 0, 55, 0, 0, 0, 0,
7, 0, 0, 0, 0, 63, 63, 33, 0, 63, 63, 0, 0, 0, 0, 0, 63, 63, 63, 0, 0, 0, 63, 63, 63, 50, 51, 52, 0, 0, 0, 0, 0, 0, 0, 55, 0, 0, 0, 0,
7, 0, 0, 0, 63, 63, 63, 41, 42, 63, 63, 63, 28, 29, 30, 63, 63, 63, 63, 0, 0, 0, 63, 63, 63, 63, 59, 60, 61, 0, 0, 28, 29, 30, 0, 63, 0, 0, 0, 0,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 0, 0, 0, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 0, 0, 0, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 0, 0, 0, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7
};

void Level3::Initialize(int lives){
    state.nextScene = -1;
    GLuint mapTextureID = Util::LoadTexture("tilesheet.png");
    state.map = new Map(LEVEL3_WIDTH, LEVEL3_HEIGHT, level3_data, mapTextureID, 1.0f, 8, 8);
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
    

    state.enemies = new Entity[LEVEL3_ENEMY_COUNT];
    GLuint enemyTextureID = Util::LoadTexture("enemy_sprites.png");
    
    for (int i = 0; i < 2; i++){
        state.enemies[i].textureID = enemyTextureID;
        if (i == 0)
            state.enemies[i].position = glm::vec3(18, -6, 0);
        else if (i == 1)
            state.enemies[i].position = glm::vec3(26, -10, 0);
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
    state.enemies[2].textureID = enemyTextureID;
    state.enemies[2].position = glm::vec3(28, -5.0f, 0);
    state.enemies[2].startPosition = state.enemies[2].position;
    state.enemies[2].entityType = ENEMY;
    state.enemies[2].aiType = KOOPA_TROOPA;
    state.enemies[2].aiState = IDLE;

    state.enemies[2].animRight = new int[2] {10, 11};
    state.enemies[2].animLeft = new int[2] {8, 9};

    state.enemies[2].animIndices = state.enemies[2].animLeft;
    state.enemies[2].animFrames = 2;
    state.enemies[2].animTime = 0;
    state.enemies[2].animCols = 4;
    state.enemies[2].animRows = 4;

    state.enemies[2].width = 0.6;
    state.enemies[2].height = 1.0f;

    state.enemies[2].acceleration = glm::vec3(0.0f, -9.81f, 0.0f);
    
    // Koopa Paratroopa
    for (int i = 3, j = -3.0f; i < LEVEL3_ENEMY_COUNT; i++, j-= 3.5f){
        state.enemies[i].textureID = enemyTextureID;
        state.enemies[i].position = glm::vec3(31.5f, j, 0);
        state.enemies[i].startPosition = state.enemies[i].position;
        state.enemies[i].entityType = ENEMY;
        state.enemies[i].aiType = KOOPA_PARATROOPA;
        state.enemies[i].aiState = FLYING;
        
        state.enemies[i].animUp = new int[2] {4, 5};
        state.enemies[i].animDown = new int[2] {4, 5};
        
        state.enemies[i].animIndices = state.enemies[i].animDown;
        state.enemies[i].animFrames = 2;
        state.enemies[i].animTime = 0;
        state.enemies[i].animCols = 4;
        state.enemies[i].animRows = 4;
        
        state.enemies[i].width = 0.6f;
        state.enemies[i].height = 1.0f;
    }
}


void Level3::Update(float deltaTime){
    state.player->Update(deltaTime, NULL, state.enemies, LEVEL3_ENEMY_COUNT, state.map);
    if (state.player->complete == true) state.nextScene = 5;
    if (state.player->end == true) state.nextScene = 4;
    
//    std::cerr << "Current scene 3\n";
    
    for (int i = 0; i < LEVEL3_ENEMY_COUNT; i++){
        state.enemies[i].Update(deltaTime, state.player, NULL, 0, state.map);
    }
    
}

void Level3::Render(ShaderProgram* program){
    state.map->Render(program);
    
    state.player->Render(program);
    
    for (int i = 0; i < LEVEL3_ENEMY_COUNT; i++){
        state.enemies[i].Render(program);
    }
}
