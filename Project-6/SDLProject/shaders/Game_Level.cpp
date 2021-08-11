#include "Game_Level.h"
#include "Util.h"
#include <SDL_mixer.h>
#define GAME_LEVEL_WIDTH 50
#define GAME_LEVEL_HEIGHT 24

#define TOTAL_ENEMY_COUNT 15
#define LEVEL_COUNT 6

#define SOUND_EFFECT_COUNT 3

class Level{
public:
    glm::vec3 center;
    bool transition;
    unsigned enemy_count;
    Entity* enemies;
    glm::vec3 camera_position;
    float width;
    float height;
    
    int top_level;
    int bottom_level;
    int left_level;
    int right_level;
    
    Level(){
        center = glm::vec3(0);
        camera_position = glm::vec3(0);
        width = 16;
        height = 11;
    }
    
    
    void Update(float deltaTime, Entity* player, Map* map){
        if (camera_position.x < center.x){
            camera_position.x += 0.125;
            return;
        }
        else if (camera_position.x > center.x){
            camera_position.x += -0.125;
            return;
        }
        else if (camera_position.y < center.y){
            camera_position.y += 0.125;
            return;
        }
        else if (camera_position.y > center.y){
            camera_position.y += -0.125;
            return;
        }
        
        player->Update(deltaTime, NULL, enemies, enemy_count, map);
        
        for (int i = 0; i < enemy_count; i++){
            enemies[i].Update(deltaTime, player, enemies, enemy_count, map);
        }
    }
};

struct LevelPosition{
    LevelPosition(){}
    LevelPosition(int top, int down, int left, int right){
        this->top = top;
        this->down = down;
        this->left = left;
        this->right = right;
    }
    int top;
    int down;
    int left;
    int right;
};

LevelPosition otherLevel[LEVEL_COUNT] = {
    LevelPosition(-1, 3, -1, 1),
    
    LevelPosition(-1, 4, 0, 2),
    
    LevelPosition(-1, 5, 1, -1),
    
    LevelPosition(0, -1, -1, 4),
    
    LevelPosition(1, -1, 3, 5),
    
    LevelPosition(2, -1, 4, -1)
};

glm::vec3 centerPosition[LEVEL_COUNT] = {
    glm::vec3(8.5, -6, 0),
    glm::vec3(24.5, -6, 0),
    glm::vec3(40.5, -6, 0),
    glm::vec3(8.5, -17, 0),
    glm::vec3(24.5, -17, 0),
    glm::vec3(40.5, -17, 0)
};

unsigned int enemyCounts[LEVEL_COUNT] = {
    3,
    2,
    4,
    3,
    0,
    3
};
glm::vec3 enemyPosition[TOTAL_ENEMY_COUNT] = {
    glm::vec3(8, -6, 0),
    glm::vec3(6, -9, 0),
    glm::vec3(1, -3, 0),
    
    glm::vec3(23.5f, -5, 0),
    glm::vec3(28, -8, 0),
    
    glm::vec3(38, -2, 0),
    glm::vec3(44, -4, 0),
    glm::vec3(45, -10, 0),
    glm::vec3(40.5f, -6, 0),
    
    glm::vec3(3, -16, 0),
    glm::vec3(9, -18, 0),
    glm::vec3(1, -20, 0),
    
    glm::vec3(36, -20, 0),
    glm::vec3(43, -17, 0),
    glm::vec3(47, -15, 0)
    
};

Level* currentLevel;
Level* Levels;

void SwitchLevels(Level* nextLevel){
    nextLevel->camera_position = currentLevel->camera_position;
    currentLevel = nextLevel;
}

void Game_Level::Initialize(int lives){
    state.nextScene = -1;
    glClearColor(0.9882f, 0.847f, 0.6588f, 1.0f);
    
    state.enemiesAlive = TOTAL_ENEMY_COUNT;
    
    unsigned int* map_data = Util::LoadMapArray("map_data.txt", GAME_LEVEL_WIDTH, GAME_LEVEL_HEIGHT);
    GLuint mapTextureID = Util::LoadTexture("Legend_of_Zelda/tilesheet.png");
    state.map = new Map(GAME_LEVEL_WIDTH, GAME_LEVEL_HEIGHT, map_data, mapTextureID, 1.0f, 5, 3);
    // Initialize Game Objects
    
    state.soundEffects = new Mix_Chunk*[SOUND_EFFECT_COUNT];
    state.soundEffects[0] = Mix_LoadWAV("Legend_of_Zelda/enemy_die.wav");
    state.soundEffects[1] = Mix_LoadWAV("Legend_of_Zelda/sword_slash.wav");
    state.soundEffects[2] = Mix_LoadWAV("Legend_of_Zelda/link_hurt.wav");
    
    Mix_VolumeChunk(state.soundEffects[2], MIX_MAX_VOLUME);

    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(24.5, -17, 0);
    state.player->startPosition = state.player->position;
    state.player->lives = lives;

    state.player->width = 0.65f;
    state.player->height = 0.65f;

    state.player->textureID = Util::LoadTexture("Legend_of_Zelda/link.png");

    state.player->animRight = new int[4]{ 2, 6, 2, 6 };
    state.player->animLeft = new int[4]{ 3, 7, 3, 7};
    state.player->animUp = new int[4]{ 1, 5, 1, 5 };
    state.player->animDown = new int[4]{ 0, 4, 0, 4 };
    
    state.player->animAttackRight = new int[4] {10, 14, 14, 10};
    state.player->animAttackLeft = new int[4] {11, 19, 19, 11};
    state.player->animAttackUp = new int[4] {9, 17, 17, 9};
    state.player->animAttackDown = new int[4] {8, 12, 12, 8};
    
    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 4;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 4;
    state.player->animRows = 6;
    
//    state.player->armed = true;
    
    // Weapon
    state.player->weapon = new Entity();
    state.player->weapon->entityType = WEAPON;
    state.player->weapon->textureID = Util::LoadTexture("Legend_of_Zelda/link.png");
    
    state.player->weapon->width = 0.65f;
    state.player->weapon->height = 0.65f;
    
    state.player->weapon->isActive = false;
    
    state.player->weapon->animRight = new int[4]{20, 15, 15, 20};
    state.player->weapon->animLeft = new int[4]{20, 18, 18, 20 };
    state.player->weapon->animUp = new int[4]{ 20, 13, 13, 20 };
    state.player->weapon->animDown = new int[4]{20, 16, 16, 20 };
    
//    state.player->weapon->animAttackRight = new int[2] {10, 14};
//    state.player->weapon->animAttackLeft = new int[2] {11, 19};
//    state.player->weapon->animAttackUp = new int[2] {9, 17};
//    state.player->weapon->animAttackDown = new int[2] {8, 12};
    
    state.player->weapon->animIndices = state.player->animRight;
    state.player->weapon->animFrames = 4;
    state.player->weapon->animIndex = 0;
    state.player->weapon->animTime = 0;
    state.player->weapon->animCols = 4;
    state.player->weapon->animRows = 6;
    
    GLuint enemyTextureID = Util::LoadTexture("Legend_of_Zelda/ai.png");
    // Making Levels and Enemies
    Levels = new Level[LEVEL_COUNT];
    int count = 0;
    for (int i = 0; i < LEVEL_COUNT; i++){
        Levels[i].center = centerPosition[i];
        Levels[i].camera_position = Levels[i].center;
        Levels[i].top_level = otherLevel[i].top;
        Levels[i].bottom_level = otherLevel[i].down;
        Levels[i].right_level = otherLevel[i].right;
        Levels[i].left_level = otherLevel[i].left;
        Levels[i].enemy_count = enemyCounts[i];
        Levels[i].enemies = new Entity[Levels[i].enemy_count];
        
        for (int j = 0; j < Levels[i].enemy_count; j++){
            Levels[i].enemies[j].entityType = ENEMY;
            Levels[i].enemies[j].textureID = enemyTextureID;
            Levels[i].enemies[j].aiType = OCTOROK;

            Levels[i].enemies[j].velocity = glm::vec3(-3, 0, 0);
            Levels[i].enemies[j].position = enemyPosition[count + j];
            
            Levels[i].enemies[j].animRight = new int[2] {4, 5};
            Levels[i].enemies[j].animLeft = new int[2] {2, 3};
            Levels[i].enemies[j].animUp = new int[2] {6, 7};
            Levels[i].enemies[j].animDown = new int[2] {0, 1};

            Levels[i].enemies[j].animIndices = Levels[i].enemies[j].animLeft;
            Levels[i].enemies[j].animFrames = 2;
            Levels[i].enemies[j].animTime = 0;
            Levels[i].enemies[j].animCols = 4;
            Levels[i].enemies[j].animRows = 2;
            
            Levels[i].enemies[j].width = 1.0;
            Levels[i].enemies[j].height = 1.0;
        }
         count += Levels[i].enemy_count;
    }
    
    currentLevel = &Levels[4];
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
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_SPACE:
                        if (state.player->armed == true && state.player->attacking == false){
                            Mix_PlayChannel(-1, state.soundEffects[1], 0);
                            state.player->attacking = true;
                            state.player->weapon->isActive = true;
                            state.player->animIndex = 0;
                            state.player->animTime = 0;
                            state.player->weapon->animIndex = 0;
                            state.player->weapon->animTime = 0;
                            if (state.player->animIndices == state.player->animRight)
                                state.player->animIndices = state.player->animAttackRight;
                            else if (state.player->animIndices == state.player->animLeft)
                                state.player->animIndices = state.player->animAttackLeft;
                            else if (state.player->animIndices == state.player->animUp)
                            state.player->animIndices = state.player->animAttackUp;
                            else if (state.player->animIndices == state.player->animDown)
                            state.player->animIndices = state.player->animAttackDown;
                        }
                }
        }
    }
    
    if (state.player->attacking == true) return;
    
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
    currentLevel->Update(deltaTime, state.player, state.map);
    
    if (state.player->position.x < currentLevel->center.x - (currentLevel->width / 2) - 0.1){
        SwitchLevels(&Levels[currentLevel->left_level]);
        state.player->startPosition = state.player->position;
    }
    else if (state.player->position.x > currentLevel->center.x + (currentLevel->width / 2) + 0.1){
        SwitchLevels(&Levels[currentLevel->right_level]);
        state.player->startPosition = state.player->position;
    }
    else if (state.player->position.y > currentLevel->center.y + (currentLevel->height / 2) + 0.1){
        SwitchLevels(&Levels[currentLevel->top_level]);
        state.player->startPosition = state.player->position;
    }
    else if (state.player->position.y < currentLevel->center.y - (currentLevel->height / 2) - 0.1){
        SwitchLevels(&Levels[currentLevel->bottom_level]);
        state.player->startPosition = state.player->position;
    }
    
    if (state.player->hurt == true){
        Mix_PlayChannel(-1, state.soundEffects[2], 0);
        state.player->hurt = false;
    }
    
    if (state.player->room == true && state.player->visited == false)
        state.nextScene = 2;
    
    if (state.player->lives == 0){
        state.nextScene = 3;
    }
    int enemiesAlive = 0;
    for (int i = 0; i < LEVEL_COUNT; i++){
        for (int j = 0; j < Levels[i].enemy_count; j++){
            if (Levels[i].enemies[j].isActive == true)
                enemiesAlive++;
        }
    }
    if (enemiesAlive < state.enemiesAlive){
        Mix_PlayChannel(-1, state.soundEffects[0], 0);
        state.enemiesAlive = enemiesAlive;
    }
    if (state.enemiesAlive == 0)
        state.nextScene = 4;

    state.viewMatrix = glm::mat4(1.0f);
    
    state.viewMatrix = glm::translate(state.viewMatrix, glm::vec3(0, -currentLevel->camera_position.y, 0));
    state.viewMatrix = glm::translate(state.viewMatrix, glm::vec3(-currentLevel->camera_position.x, 0, 0));
    
    if (state.nextScene != -1){
        Mix_FreeChunk(state.soundEffects[0]);
        Mix_FreeChunk(state.soundEffects[1]);
        Mix_FreeChunk(state.soundEffects[2]);
    }
    
}

void Game_Level::Render(ShaderProgram* program){
    state.map->Render(program);
    
    state.player->Render(program);
    state.player->weapon->Render(program);
    
    
    for (int i = 0; i < currentLevel->enemy_count; i++){
        currentLevel->enemies[i].Render(program);
    }
}
