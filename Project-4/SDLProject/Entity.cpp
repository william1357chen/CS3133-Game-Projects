#include "Entity.h"
#include <iostream>
#include <string>
#include <vector>

Entity::Entity()
{
    position = glm::vec3(0);
    velocity.x = 0;
    acceleration = glm::vec3(0);
    velocity = glm::vec3(0);
    movement = glm::vec3(0);
    speed = 1;
    
    modelMatrix = glm::mat4(1.0f);
}

void Entity::AI(float deltaTime, Entity* player, Entity* platforms, int platformCount) {
    switch (aiType)
    {
    case GOOMBA:
        AIGoomba();
        break;
    case KOOPA_TROOPA:
        AIKoopaTroopa(player);
        break;
    case KOOPA_PARATROOPA:
        AIKoopaParatroopa();
        break;
    }
    velocity += acceleration * deltaTime;

    position.y += velocity.y * deltaTime; // Move on Y
    CheckCollisionsY(platforms, platformCount);// Fix if needed
    position.x += velocity.x * deltaTime; // Move on X
    CheckCollisionsX(platforms, platformCount);// Fix if needed
    if (aiType == GOOMBA){
        if (collidedRight == true){
            velocity.x = -0.5f;
        }
        if (collidedLeft == true)
            velocity.x = 0.5f;
    }
}

void Entity::AIGoomba() {
//    velocity.x = 0.5f;
}

void Entity::AIKoopaTroopa(Entity* player) {
    switch (aiState)
    {
        case IDLE:
            if (glm::distance(position, player->position) < 3.0f)
                aiState = WALKING;
            break;
        case WALKING:
            if (player->position.x < position.x){
                velocity.x = -0.5f;
                if (animIndex == 0)
                    animIndices = animLeft;
            }
            else{
                velocity.x = 0.5f;
                if (animIndex == 0)
                animIndices = animRight;
            }
            break;
        default:
            break;
    }
}

void Entity::AIKoopaParatroopa(){
    switch (aiState){
        case FLYING:
            if (position.y >= 0.0f)
                velocity.y = -0.5f;
            if (position.y <= -1.5f)
                velocity.y = 0.5f;
            break;
        default:
            break;
            
    }
}

void Entity::Player(float deltaTime, Entity* platforms, int platformCount, Entity* enemies, int enemyCount){
    if (jump) {
        jump = false;
        velocity.y += jumpPower;
    }
    // Implement friction
    if (entityType == PLAYER){
        float friction = 0.05f;
        
        if (velocity.x > 5.0f) velocity.x = 5.0f;
        if (velocity.x < -5.0f) velocity.x = -5.0f;
        
        if (velocity.x > 0) {
            velocity.x -= friction;
            if (velocity.x < 0)
                velocity.x = 0;
        }
        
        if (velocity.x < 0) {
            velocity.x += friction;
            if (velocity.x > 0)
                velocity.x = 0;
        }
    }
    velocity += acceleration * deltaTime;

    position.y += velocity.y * deltaTime; // Move on Y
    CheckCollisionsY(platforms, platformCount);// Fix if needed
    CheckCollisionsY(enemies, enemyCount);
    
    position.x += velocity.x * deltaTime; // Move on X
    CheckCollisionsX(platforms, platformCount);// Fix if needed
    CheckCollisionsX(enemies, enemyCount);
}


void Entity::Update(float deltaTime, Entity* player, Entity* platforms, int platformCount, Entity* enemies, int enemyCount)
{
    if (isActive == false) return;

    collidedTop = false;
    collidedBottom = false;
    collidedLeft = false;
    collidedRight = false;
    
    switch (entityType){
        case ENEMY:
            AI(deltaTime, player, platforms, platformCount);
            break;
        case PLAYER:
            Player(deltaTime, platforms, platformCount, enemies, enemyCount);
            break;
        case PLATFORM:
            break;
        default:
            break;
    }

    if (animIndices != NULL) {
        if (glm::length(velocity.x) != 0 || aiType == KOOPA_PARATROOPA) {
            animTime += deltaTime;

            if (animTime >= 0.25f)
            {
                animTime = 0.0f;
                animIndex++;
                if (animIndex >= animFrames)
                {
                    animIndex = 0;
                }
            }
        }
        else {
            animIndex = 0;
        }
    }

    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);

}

void Entity::CheckCollisionsY(Entity* objects, int objectCount)
{
    for (int i = 0; i < objectCount; i++)
    {
        Entity* object = &objects[i];
        if (CheckCollision(object))
        {
            float ydist = fabs(position.y - object->position.y);
            float penetrationY = fabs(ydist - (height / 2.0f) - (object->height / 2.0f));
            if (velocity.y > 0 || object->velocity.y < 0) {
                position.y -= penetrationY;
                velocity.y = 0;
                collidedTop = true;
                
                if (object->entityType == ENEMY && this->entityType == PLAYER){
                    this->isActive = false;
                }
            }
            else if (velocity.y < 0 || object->velocity.y > 0) {
                position.y += penetrationY;
                velocity.y = 0;
                collidedBottom = true;
                
                if (object->entityType == ENEMY && this->entityType == PLAYER){
                    // object->animIndices = object->animDead;
                    this->velocity.y = 3.0f;
                    object->isActive = false;
                }
            }
        }
    }
}

void Entity::CheckCollisionsX(Entity* objects, int objectCount)
{
    for (int i = 0; i < objectCount; i++)
    {
        Entity* object = &objects[i];
        if (CheckCollision(object))
        {
            float xdist = fabs(position.x - object->position.x);
            float penetrationX = fabs(xdist - (width / 2.0f) - (object->width / 2.0f));
            if (velocity.x > 0 || object->velocity.x < 0) {
                position.x -= penetrationX;
                velocity.x = 0;
                collidedRight = true;
                if (object->entityType == ENEMY && this->entityType == PLAYER){
                    this->isActive = false;
                }
            }
            else if (velocity.x < 0 || object->velocity.x > 0) {
                position.x += penetrationX;
                velocity.x = 0;
                collidedLeft = true;
                if (object->entityType == ENEMY && this->entityType == PLAYER){
                    this->isActive = false;
                }
            }
        }
    }
}
void Entity::DrawSpriteFromTextureAtlas(ShaderProgram *program, GLuint textureID, int index)
{
    float u = (float)(index % animCols) / (float)animCols;
    float v = (float)(index / animCols) / (float)animRows;
    
    float width = 1.0f / (float)animCols;
    float height = 1.0f / (float)animRows;
    
    float texCoords[] = { u, v + height, u + width, v + height, u + width, v,
        u, v + height, u + width, v, u, v};
    
    float vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void Entity::Render(ShaderProgram *program) {

    if (isActive == false) return;
    program->SetModelMatrix(modelMatrix);
    
    if (entityType == TEXT) {
        DrawText(program, textureID, text, 0.75f, -0.25f, position);
        return;
    }

    if (animIndices != NULL) {
        DrawSpriteFromTextureAtlas(program, textureID, animIndices[animIndex]);
        return;
    }
    
        float vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
        float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
        
        glBindTexture(GL_TEXTURE_2D, textureID);
        
        glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
        glEnableVertexAttribArray(program->positionAttribute);
        
        glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
        glEnableVertexAttribArray(program->texCoordAttribute);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        glDisableVertexAttribArray(program->positionAttribute);
        glDisableVertexAttribArray(program->texCoordAttribute);
    
    
}

bool Entity::CheckCollision(Entity* other) {

    if (isActive == false || other->isActive == false) return false;

    float xdist = fabs(position.x - other->position.x) - ((width + other->width) / 2.0f);
    float ydist = fabs(position.y - other->position.y) - ((height + other->height) / 2.0f);
    return xdist < 0 && ydist < 0;
}

void Entity::DrawText(ShaderProgram* program, GLuint fontTextureID, std::string text,
    float size, float spacing, glm::vec3 position)
{
    float width = 1.0f / 16.0f;
    float height = 1.0f / 16.0f;
    std::vector<float> vertices;
    std::vector<float> texCoords;
    for (int i = 0; i < text.size(); i++) {
        int index = (int)text[i];
        float offset = (size + spacing) * i;
        float u = (float)(index % 16) / 16.0f;
        float v = (float)(index / 16) / 16.0f;
        vertices.insert(vertices.end(), {
            offset + (-0.5f * size), 0.5f * size,
            offset + (-0.5f * size), -0.5f * size,
            offset + (0.5f * size), 0.5f * size,
            offset + (0.5f * size), -0.5f * size,
            offset + (0.5f * size), 0.5f * size,
            offset + (-0.5f * size), -0.5f * size,
        });
        texCoords.insert(texCoords.end(), {
            u, v,
            u, v + height,
            u + width, v,
            u + width, v + height,
            u + width, v,
            u, v + height,
            });
   } // end of for loop
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    program->SetModelMatrix(modelMatrix);
    glUseProgram(program->programID);
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->positionAttribute);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords.data());
    glEnableVertexAttribArray(program->texCoordAttribute);
    glBindTexture(GL_TEXTURE_2D, fontTextureID);
    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}
