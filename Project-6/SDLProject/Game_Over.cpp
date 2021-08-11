//
//  Game_Over.cpp
//  SDLProject
//
//  Created by William Chen on 2021/7/26.
//  Copyright © 2021 ctg. All rights reserved.
//

#include "Game_Over.h"
#include "Util.h"
#include <string>

#define MESSAGE_COUNT 2

void Game_Over::Initialize(int lives){
    state.nextScene = -1;
    
    GLuint fontTextureID = Util::LoadTexture("font.png");
    state.message = new Entity[MESSAGE_COUNT];
    state.message[0].text = "You Lose!";
    state.message[0].entityType = TEXT;
    state.message[0].textureID = fontTextureID;
    state.message[0].position = glm::vec3(-5.0f, 1, 0);
    state.message[0].width = 1.50f;
    state.message[0].Update(0, nullptr, nullptr, 0, 0);
    
    state.message[1].text = "Try again by Pressing Enter";
    state.message[1].entityType = TEXT;
    state.message[1].textureID = fontTextureID;
    state.message[1].position = glm::vec3(-6.5f, -3.5f, 0);
    state.message[1].width = 0.75f;
    state.message[1].Update(0, nullptr, nullptr, 0, 0);
    
}

void Game_Over::ProcessInput(){
    SDL_Event event;
    while (SDL_PollEvent(&event)){
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                state.gameIsRunning = false;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_RETURN:
                        state.nextScene = 0;
                    default:
                    break;
                }
        }
    }
}

void Game_Over::Update(float deltaTime){
}

void Game_Over::Render(ShaderProgram* program){
    for (int i = 0; i < MESSAGE_COUNT; i++){
        state.message[i].Render(program);
    }
}
