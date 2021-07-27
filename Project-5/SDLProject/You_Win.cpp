#include "You_Win.h"
#include "Util.h"
#include <string>

#define MESSAGE_COUNT 2

void You_Win::Initialize(int lives){
    state.nextScene = -1;
    
    GLuint fontTextureID = Util::LoadTexture("font.png");
    state.message = new Entity[MESSAGE_COUNT];
    state.message[0].text = "You Win!";
    state.message[0].entityType = TEXT;
    state.message[0].textureID = fontTextureID;
    state.message[0].position = glm::vec3(-4.5f, 1, 0);
    state.message[0].width = 1.5f;
    state.message[0].Update(0, nullptr, nullptr, 0, 0);
    
    state.message[1].text = "Play Again by Pressing Enter";
    state.message[1].entityType = TEXT;
    state.message[1].textureID = fontTextureID;
    state.message[1].position = glm::vec3(-7.0f, -3.5f, 0);
    state.message[1].width = 0.75f;
    state.message[1].Update(0, nullptr, nullptr, 0, 0);
    
}

void You_Win::Update(float deltaTime){
}

void You_Win::Render(ShaderProgram* program){
    for (int i = 0; i < MESSAGE_COUNT; i++){
        state.message[i].Render(program);
    }
}
