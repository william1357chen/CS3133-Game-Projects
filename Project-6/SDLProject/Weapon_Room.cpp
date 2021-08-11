# include "Weapon_Room.h"
# include "Map.h"
# define ROOM_WIDTH 16
# define ROOM_HEIGHT 11
# define ENEMY_COUNT 4 // not actually an enemy but the wizard, fire, sword
# define SOUND_EFFECT_COUNT 2
# include <SDL_mixer.h>
unsigned int map_data[] {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1
};

void Weapon_Room::Initialize(int lives){
    state.nextScene = -1;
    glClearColor(0, 0, 0, 1);
    
    GLuint mapTextureID = Util::LoadTexture("Legend_of_Zelda/weapon_room.png");
    state.map = new Map(ROOM_WIDTH, ROOM_HEIGHT, map_data, mapTextureID, 1.0f, 4, 2);
    
    // Initialize Game Objects
    state.soundEffects = new Mix_Chunk*[SOUND_EFFECT_COUNT];
    state.soundEffects[0] = Mix_LoadWAV("Legend_of_Zelda/item.wav");
    state.soundEffects[1] = Mix_LoadWAV("Legend_of_Zelda/sword_slash.wav");
    
    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(7.5f, -10, 0);
    state.player->startPosition = state.player->position;
    state.player->lives = lives;

    state.player->width = 1.0f;
    state.player->height = 1.0f;

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
    
    GLuint itemTextureID = mapTextureID;
    
    state.enemies = new Entity[ENEMY_COUNT];
    state.enemies[0].entityType = ENEMY;
    state.enemies[0].aiType = WIZARD;
    state.enemies[0].position = glm::vec3(7.5f, -4, 0);
    state.enemies[0].width = 1.0f;
    state.enemies[0].height = 1.0f;
    state.enemies[0].textureID = itemTextureID;
    state.enemies[0].animDown = new int[2]{ 6, 6 };
    state.enemies[0].animIndices = state.enemies[0].animDown;
    state.enemies[0].animFrames = 2;
    state.enemies[0].animIndex = 0;
    state.enemies[0].animTime = 0;
    state.enemies[0].animCols = 4;
    state.enemies[0].animRows = 2;
    
    state.enemies[1].entityType = ENEMY;
    state.enemies[1].aiType = FIRE;
    state.enemies[1].position = glm::vec3(4.5f, -4, 0);
    state.enemies[1].width = 1.0f;
    state.enemies[1].height = 1.0f;
    state.enemies[1].textureID = itemTextureID;
    state.enemies[1].animDown = new int[2]{ 4, 5 };
    state.enemies[1].animIndices = state.enemies[1].animDown;
    state.enemies[1].animFrames = 2;
    state.enemies[1].animIndex = 0;
    state.enemies[1].animTime = 0;
    state.enemies[1].animCols = 4;
    state.enemies[1].animRows = 2;
    
    state.enemies[2].entityType = ENEMY;
    state.enemies[2].aiType = FIRE;
    state.enemies[2].position = glm::vec3(10.5f, -4, 0);
    state.enemies[2].width = 1.0f;
    state.enemies[2].height = 1.0f;
    state.enemies[2].textureID = itemTextureID;
    state.enemies[2].animDown = new int[2]{ 4, 5 };
    state.enemies[2].animIndices = state.enemies[2].animDown;
    state.enemies[2].animFrames = 2;
    state.enemies[2].animIndex = 0;
    state.enemies[2].animTime = 0;
    state.enemies[2].animCols = 4;
    state.enemies[2].animRows = 2;
    
    state.enemies[3].entityType = ENEMY;
    state.enemies[3].aiType = SWORD;
    state.enemies[3].position = glm::vec3(7.5f, -6, 0);
    state.enemies[3].width = 1.0f;
    state.enemies[3].height = 1.0f;
    state.enemies[3].textureID = itemTextureID;
    state.enemies[3].animDown = new int[2]{ 7, 7 };
    state.enemies[3].animIndices = state.enemies[3].animDown;
    state.enemies[3].animFrames = 2;
    state.enemies[3].animIndex = 0;
    state.enemies[3].animTime = 0;
    state.enemies[3].animCols = 4;
    state.enemies[3].animRows = 2;
    
    
}

void Weapon_Room::ProcessInput() {
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

void Weapon_Room::Update(float deltaTime){
    state.player->Update(deltaTime, NULL, state.enemies, ENEMY_COUNT, state.map);
    
    for (int i = 0; i < ENEMY_COUNT; i++){
        state.enemies[i].Update(deltaTime, state.player, NULL, 0, state.map);
    }
    
    if (state.enemies[3].isActive == false && state.soundEffects[0] != NULL){
        Mix_PlayChannel(-1, state.soundEffects[0], 0);
        state.soundEffects[0] = NULL;
    }
    
    if (state.player->position.y < -12){
        state.nextScene = 1;
        
        Mix_FreeChunk(state.soundEffects[1]);
    }
    
    state.viewMatrix = glm::mat4(1.0f);
    state.viewMatrix = glm::translate(state.viewMatrix, glm::vec3(-7.5, 5, 0));
}
void Weapon_Room::Render(ShaderProgram *program){
    state.map->Render(program);
    
    state.player->Render(program);
    
    state.player->weapon->Render(program);
    
    for (int i = 0; i < ENEMY_COUNT; i++){
        state.enemies[i].Render(program);
    }
}
