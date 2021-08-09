//
//  Weapon_Room.cpp
//  SDLProject
//
//  Created by William Chen on 2021/8/7.
//  Copyright © 2021 ctg. All rights reserved.
//

#include "Weapon_Room.h"

void Weapon_Room::Initialize(int lives){
    state.nextScene = -1;
    //GLuint mapTextureID = Util::LoadTexture("tilesheet.png");
    // state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, level1_data, mapTextureID, 1.0f, 8, 8);
    // Initialize Game Objects

    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(0, 0, 0);
    state.player->startPosition = state.player->position;
    state.player->lives = lives;

    state.player->width = 0.90f;
    state.player->height = 0.90f;

    state.player->textureID = Util::LoadTexture("Legend_of_Zelda/link.png");

    state.player->animRight = new int[2]{ 2, 6};
    state.player->animLeft = new int[2]{ 3, 7};
    state.player->animUp = new int[2]{ 1, 5 };
    state.player->animDown = new int[2]{ 0, 4 };
    
    state.player->animIndices = state.player->animUp;
    state.player->animFrames = 2;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 4;
    state.player->animRows = 5;
}
void Weapon_Room::ProcessInput(){
    
}
void Weapon_Room::Update(float deltaTime){
    
}
void Weapon_Room::Render(ShaderProgram *program){
    
}
