
#include "Start_Menu.h"
#include "Util.h"
#include <string>

#define MESSAGE_COUNT 2

void Start_Menu::Initialize(int lives){
    state.nextScene = -1;
    
    GLuint fontTextureID = Util::LoadTexture("font.png");
    state.message = new Entity[MESSAGE_COUNT];
    state.message[0].text = "The Legend of Zelda";
    state.message[0].entityType = TEXT;
    state.message[0].textureID = fontTextureID;
    state.message[0].position = glm::vec3(-7.25, 1, 0);
    state.message[0].width = 1.0f;
    state.message[0].Update(0, nullptr, nullptr, 0, 0);
    
    state.message[1].text = "Press Enter to Start";
    state.message[1].entityType = TEXT;
    state.message[1].textureID = fontTextureID;
    state.message[1].position = glm::vec3(-5.0f, -3.5f, 0);
    state.message[1].width = 0.75f;
    state.message[1].Update(0, nullptr, nullptr, 0, 0);
    
}
void Start_Menu::ProcessInput(){
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
                        state.nextScene = 1;
                    default:
                    break;
                }
        }
    }
}

void Start_Menu::Update(float deltaTime){
}

void Start_Menu::Render(ShaderProgram* program){
    for (int i = 0; i < MESSAGE_COUNT; i++){
        state.message[i].Render(program);
    }
}
