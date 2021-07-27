#include "Entity.h"
#include "Map.h"
#include "Util.h"
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
    startPosition = glm::vec3(0);
    modelMatrix = glm::mat4(1.0f);
}

void Entity::AI(float deltaTime, Entity* player, Map* map) {
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
    CheckCollisionsY(map);// Fix if needed
    position.x += velocity.x * deltaTime; // Move on X
    CheckCollisionsX(map);// Fix if needed
    
//    if (position.y < -14.0f){
//        if (entityType == ENEMY)
//            isActive = false;
//    }
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
            if (glm::distance(position, player->position) < 5.0f)
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
            if (startPosition.y - position.y <= 0)
                velocity.y = -0.5f;
            if (startPosition.y - position.y >= 3.0f)
                velocity.y = 0.5f;
            break;
        default:
            break;
            
    }
}

void Entity::Player(float deltaTime, Entity* enemies, int enemyCount, Map* map){
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
//    std::cerr << "Velocity y is: " << velocity.y << '\n';
//    std::cerr << "Jump is: " << jump << '\n';
//    std::cerr << "SpaceOn is: " << spaceOn << '\n';
//    std::cerr << "AirTime is: " << airTime << '\n';
    
    if (jump == true && spaceOn == true){
        airTime += deltaTime;
        if (airTime >= 0.55){
            airTime = 0;
            jump = false;
        }
        else
            velocity.y += 0.26;
    }
    else{
        jump = false;
        airTime = 0;
    }
    
    velocity += acceleration * deltaTime;

    position.y += velocity.y * deltaTime; // Move on Y
    CheckCollisionsY(map);
    CheckCollisionsY(enemies, enemyCount); // Fix if needed
    position.x += velocity.x * deltaTime; // Move on X
    CheckCollisionsX(map);
    CheckCollisionsX(enemies, enemyCount);
    
    if (position.y < -14.0f){
        lives--;
        position = startPosition;
    }
    
    std::cerr << "Position x is: " << position.x << '\n';
    std::cerr << "Position y is: " << position.y << '\n';
    if (lives == 0){
        end = true;
    }
//    std::cerr << "Current Lives: " << lives << '\n';
}


void Entity::Update(float deltaTime, Entity* player, Entity* enemies, int enemyCount, Map* map)
{
    if (isActive == false) return;
    modelMatrix = glm::mat4(1.0f);
    collidedTop = false;
    collidedBottom = false;
    collidedLeft = false;
    collidedRight = false;
    
    switch (entityType){
        case ENEMY:
            AI(deltaTime, player, map);
            break;
        case PLAYER:
            Player(deltaTime, enemies, enemyCount, map);
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
            if (velocity.y > 0 || (object->velocity.y < 0 && velocity.x == 0)) {
                position.y -= penetrationY;
                velocity.y = 0;
                collidedTop = true;
                
                if (object->entityType == ENEMY && this->entityType == PLAYER){
                    this->lives -= 1;
                    position = startPosition;
//                    std::cerr << "Collided Top \n";
                }
            }
            else if (velocity.y < 0 || (object->velocity.y > 0 && velocity.y == 0)) {
                position.y += penetrationY;
                velocity.y = 0;
                collidedBottom = true;
                
                if (object->entityType == ENEMY && this->entityType == PLAYER){
                    // object->animIndices = object->animDead;
                    this->velocity.y = 3.0f;
                    object->isActive = false;
//                    std::cerr << "Collided Bottom \n";
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
            if (velocity.x > 0 || (object->velocity.x < 0 && velocity.x == 0)) {
                position.x -= penetrationX;
                velocity.x = 0;
                collidedRight = true;
                if (object->entityType == ENEMY && this->entityType == PLAYER){
                    this->lives -= 1;
                    position = startPosition;
                }
            }
            else if (velocity.x < 0 || (object->velocity.x > 0 && velocity.x == 0)) {
                position.x += penetrationX;
                velocity.x = 0;
                collidedLeft = true;
                if (object->entityType == ENEMY && this->entityType == PLAYER){
                    this->lives -= 1;
                    position = startPosition;
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
        Util::DrawText(program, textureID, text, width, -0.25f, position);
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

void Entity::CheckCollisionsY(Map* map)
{
    // Probes for tiles
    glm::vec3 top = glm::vec3(position.x, position.y + (height / 2), position.z);
    glm::vec3 top_left = glm::vec3(position.x - (width / 2) + 0.05f, position.y + (height / 2), position.z);
    glm::vec3 top_right = glm::vec3(position.x + (width / 2) - 0.05f, position.y + (height / 2), position.z);
    glm::vec3 bottom = glm::vec3(position.x, position.y - (height / 2), position.z);
    glm::vec3 bottom_left = glm::vec3(position.x - (width / 2) + 0.05f, position.y - (height / 2), position.z);
    glm::vec3 bottom_right = glm::vec3(position.x + (width / 2) - 0.05f, position.y - (height / 2), position.z);
    int pole = 0;
    float penetration_x = 0;
    float penetration_y = 0;
    if (map->IsSolid(top, &penetration_x, &penetration_y, &pole) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
    }
    else if (map->IsSolid(top_left, &penetration_x, &penetration_y, &pole) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
    }
    else if (map->IsSolid(top_right, &penetration_x, &penetration_y, &pole) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
    }
    if (map->IsSolid(bottom, &penetration_x, &penetration_y, &pole) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
    }
    else if (map->IsSolid(bottom_left, &penetration_x, &penetration_y, &pole) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
    }
    else if (map->IsSolid(bottom_right, &penetration_x, &penetration_y, &pole) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
    }
    
    if (pole == 1 && this->entityType == PLAYER) complete = true;
    
    if (this->entityType == PLAYER && collidedBottom == true){
        jump = false;
        if (animIndices == animJumpLeft)
            animIndices = animLeft;
        else if (animIndices == animJumpRight)
            animIndices = animRight;
    }
}

void Entity::CheckCollisionsX(Map *map)
{
    // Probes for tiles
    glm::vec3 left = glm::vec3(position.x - (width / 2), position.y, position.z);
    glm::vec3 right = glm::vec3(position.x + (width / 2), position.y, position.z);
    float penetration_x = 0;
    float penetration_y = 0;
    int pole = 0;
    if (map->IsSolid(left, &penetration_x, &penetration_y, &pole) && velocity.x < 0) {
        position.x += penetration_x;
        velocity.x = 0;
        collidedLeft = true;
    }
    if (map->IsSolid(right, &penetration_x, &penetration_y, &pole) && velocity.x > 0) {
        position.x -= penetration_x;
        velocity.x = 0;
        collidedRight = true;
    }
    if (pole == 1 && this->entityType == PLAYER) complete = true;
}
