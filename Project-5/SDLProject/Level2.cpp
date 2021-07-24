#include "Level2.h"
#include "Util.h"
#define LEVEL2_WIDTH 14
#define LEVEL2_HEIGHT 8

#define LEVEL2_ENEMY_COUNT 1

unsigned int level2_data[] =
{
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
    3, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2,
    3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
};

void Level2::Initialize(){
    state.nextScene = -1;
    GLuint mapTextureID = Util::LoadTexture("blocks.png");
    state.map = new Map(LEVEL2_WIDTH, LEVEL2_HEIGHT, level2_data, mapTextureID, 1.0f, 1, 1);
    // Initialize Game Objects

    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(-4, -2, 0);
    state.player->movement = glm::vec3(0);
    state.player->speed = 3.0f;

    state.player->acceleration = glm::vec3(0, -9.81f, 0);
    state.player->jumpPower = 5.0f;

    state.player->width = 0.5f;
    state.player->height = 0.5f;

    state.player->textureID = Util::LoadTexture("mario_sprite.png");

    state.player->animRight = new int[4]{ 4, 5, 6, 7 };
    state.player->animLeft = new int[4]{ 3, 2, 1, 0 };
    state.player->animJumpLeft = new int[4]{8, 8, 8, 8};
    state.player->animJumpRight = new int[4]{9, 9, 9, 9};
    

    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 4;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 4;
    state.player->animRows = 4;
    

    state.enemies = new Entity[LEVEL2_ENEMY_COUNT];
    GLuint enemyTextureID = Util::LoadTexture("enemies_sprites.png");
    
    state.enemies[0].textureID = enemyTextureID;
    state.enemies[0].position = glm::vec3(-1.25, -2.25f, 0);
    state.enemies[0].entityType = ENEMY;
    state.enemies[0].aiType = GOOMBA;
    state.enemies[0].aiState = WALKING;
    state.enemies[0].velocity.x = -0.5f;
    
    state.enemies[0].animRight = new int[2] {1, 0};
    state.enemies[0].animLeft = new int[2] {0, 1};
    
    state.enemies[0].animIndices = state.enemies[0].animLeft;
    state.enemies[0].animFrames = 2;
    state.enemies[0].animTime = 0;
    state.enemies[0].animCols = 4;
    state.enemies[0].animRows = 4;
    
    state.enemies[0].width = 0.5;
    state.enemies[0].height = 0.5;
    
    state.enemies[0].acceleration = glm::vec3(0.0f, -9.81f, 0.0f);

    // Koopa Troopa
    state.enemies[1].textureID = enemyTextureID;
    state.enemies[1].position = glm::vec3(4, -2.25f, 0);
    state.enemies[1].entityType = ENEMY;
    state.enemies[1].aiType = KOOPA_TROOPA;
    state.enemies[1].aiState = IDLE;
    
    state.enemies[1].animRight = new int[2] {10, 11};
    state.enemies[1].animLeft = new int[2] {8, 9};
    
    state.enemies[1].animIndices = state.enemies[1].animLeft;
    state.enemies[1].animFrames = 2;
    state.enemies[1].animTime = 0;
    state.enemies[1].animCols = 4;
    state.enemies[1].animRows = 4;
    
    state.enemies[1].width = 0.5;
    state.enemies[1].height = 0.8;
    
    state.enemies[1].acceleration = glm::vec3(0.0f, -9.81f, 0.0f);
    
    // Koopa Paratroopa
    state.enemies[2].textureID = enemyTextureID;
    state.enemies[2].position = glm::vec3(2, 0, 0);
    state.enemies[2].entityType = ENEMY;
    state.enemies[2].aiType = KOOPA_PARATROOPA;
    state.enemies[2].aiState = FLYING;
    
    state.enemies[2].animUp = new int[2] {4, 5};
    state.enemies[2].animDown = new int[2] {4, 5};
    
    state.enemies[2].animIndices = state.enemies[2].animDown;
    state.enemies[2].animFrames = 2;
    state.enemies[2].animTime = 0;
    state.enemies[2].animCols = 4;
    state.enemies[2].animRows = 4;
    
    state.enemies[2].width = 0.5;
    state.enemies[2].height = 0.8;
}


void Level2::Update(float deltaTime){
    state.player->Update(deltaTime, NULL, state.enemies, LEVEL2_ENEMY_COUNT, state.map);
    
    for (int i = 0; i < LEVEL2_ENEMY_COUNT; i++){
        state.enemies[i].Update(deltaTime, state.player, NULL, 0, state.map);
    }
    
}

void Level2::Render(ShaderProgram* program){
    state.map->Render(program);
    
    state.player->Render(program);
    
    for (int i = 0; i < LEVEL2_ENEMY_COUNT; i++){
        state.enemies[i].Render(program);
    }
}
