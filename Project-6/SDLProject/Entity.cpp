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
    velocity = glm::vec3(0);
    startPosition = glm::vec3(0);
    modelMatrix = glm::mat4(1.0f);
}

void Entity::AI(float deltaTime, Entity* player, Entity* enemies, int enemyCount, Map* map) {
    switch (aiType)
    {
        case OCTOROK:
            break;
        default:
            break;
    }
    if (aiType == SWORD){
        if (player->visited == true)
            isActive = false;
    }

    position.y += velocity.y * deltaTime; // Move on Y
    CheckCollisionsY(map);// Fix if needed
    CheckCollisionsY(enemies, enemyCount);
    position.x += velocity.x * deltaTime; // Move on X
    CheckCollisionsX(map);// Fix if needed
    CheckCollisionsX(enemies, enemyCount);
    
    ChangeDirection();
}

void Entity::ChangeDirection(){
    int num;
    if (collidedTop || collidedBottom || collidedLeft || collidedRight){
        num = rand() % 4;
        switch (num) {
            case 0: // top
                velocity = glm::vec3(0, 3.0f, 0);
                animIndices = animUp;
                break;
            case 1: // down
                velocity = glm::vec3(0, -3.0f, 0);
                animIndices = animDown;
                break;
            case 2: // left
                velocity = glm::vec3(-3.0f, 0, 0);
                animIndices = animLeft;
                break;
            case 3:  // right
                velocity = glm::vec3(3.0f, 0, 0);
                animIndices = animRight;
                break;
        }
    }
    else{
        return;
    }
}

void Entity::Player(float deltaTime, Entity* enemies, int enemyCount, Map* map){
    
    room = false;
    
    if (attacking == true)
        weapon->Update(deltaTime, this, enemies, enemyCount, map);
    else{
        position.y += velocity.y * deltaTime; // Move on Y
        CheckCollisionsY(map);
        CheckCollisionsY(enemies, enemyCount); // Fix if needed
        position.x += velocity.x * deltaTime; // Move on X
        CheckCollisionsX(map);
        CheckCollisionsX(enemies, enemyCount);
    }
    
//    std::cerr << "Player Position x is: " << position.x << '\n';
//    std::cerr << "Player Position y is: " << position.y << '\n';
//    std::cerr << "Player Current Active : " << isActive << '\n';
}

void Entity::Weapon(float deltaTime, Entity* player, Entity* enemies, int enemyCount, Map* map){
    if (attacking  == false){
        if (player->animIndices == player->animAttackRight){
            this->animIndices = this->animRight;
            this->position = glm::vec3(player->position.x + 1, player->position.y, 0);
        }
        else if (player->animIndices == player->animAttackLeft){
            this->animIndices = this->animLeft;
            this->position = glm::vec3(player->position.x - 1, player->position.y, 0);
        }
        else if (player->animIndices == player->animAttackUp){
            this->animIndices = this->animUp;
            this->position = glm::vec3(player->position.x, player->position.y + 1, 0);
        }
        else if (player->animIndices == player->animAttackDown){
            this->animIndices = this->animDown;
            this->position = glm::vec3(player->position.x, player->position.y - 1, 0);
        }
        attacking = true;
    }
    
    CheckCollisionsY(enemies, enemyCount);
    CheckCollisionsX(enemies, enemyCount);
    
    
//    std::cerr << "Weapon Position x is: " << position.x << '\n';
//    std::cerr << "Weapon Position y is: " << position.y << '\n';
//    std::cerr << "Weapon Current Active : " << isActive << '\n';
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
            AI(deltaTime, player, enemies, enemyCount, map);
            break;
        case PLAYER:
            Player(deltaTime, enemies, enemyCount, map);
            break;
        case WEAPON:
            Weapon(deltaTime, player, enemies, enemyCount, map);
            break;
        default:
            break;
    }

    if (animIndices != NULL) {
        if (glm::length(velocity) != 0 || aiType == FIRE || entityType == WEAPON || attacking == true) {
            animTime += deltaTime;
            
            if (entityType == PLAYER && attacking == true && animIndex == 3){
                attacking = false;
                weapon->isActive = false;
                weapon->attacking = false;
                if (animIndices == animAttackRight)
                    animIndices = animRight;
                else if (animIndices == animAttackLeft)
                    animIndices = animLeft;
                else if (animIndices == animAttackUp)
                    animIndices = animUp;
                else if (animIndices == animAttackDown)
                    animIndices = animDown;
            }
//            if (entityType == PLAYER){
//                std::cerr << "Index for Player: " << animIndex << std::endl;
//            }
//            if (entityType == WEAPON){
//                std::cerr << "Index for Weapon: " << animIndex << std::endl;
//            }

            if (animTime >= 0.10f)
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
        if (CheckCollision(object) && object != this)
        {
            if (entityType == WEAPON && object->aiType == OCTOROK){
                object->isActive = false;
                return;
            }
            if (entityType == PLAYER && object->aiType == SWORD){
                this->armed = true;
                object->isActive = false;
                this->visited = true;
                return;
            }
            
            float ydist = fabs(position.y - object->position.y);
            float penetrationY = fabs(ydist - (height / 2.0f) - (object->height / 2.0f));
            if (velocity.y > 0 || (object->velocity.y < 0 && velocity.x == 0)) {
                position.y -= penetrationY;
                velocity.y = 0;
                collidedTop = true;
                if (entityType == PLAYER && object->aiType == OCTOROK){
                    this->position = this->startPosition;
                    lives--;
                    hurt = true;
                }
            
            }
            else if (velocity.y < 0 || (object->velocity.y > 0 && velocity.y == 0)) {
                position.y += penetrationY;
                velocity.y = 0;
                collidedBottom = true;
                if (entityType == PLAYER && object->aiType == OCTOROK){
                    this->position = this->startPosition;
                    lives--;
                    hurt = true;
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
        if (CheckCollision(object) && object != this)
        {
            if (entityType == WEAPON && object->aiType == OCTOROK){
                object->isActive = false;
                return;
            }
            if (entityType == PLAYER && object->aiType == SWORD){
                this->armed = true;
                object->isActive = false;
            }
            
            float xdist = fabs(position.x - object->position.x);
            float penetrationX = fabs(xdist - (width / 2.0f) - (object->width / 2.0f));
            if (velocity.x > 0 || (object->velocity.x < 0 && velocity.x == 0)) {
                position.x -= penetrationX;
                velocity.x = 0;
                collidedRight = true;
                
                if (entityType == PLAYER && object->aiType == OCTOROK){
                    this->position = this->startPosition;
                    lives--;
                    hurt = true;
                }
            }
            else if (velocity.x < 0 || (object->velocity.x > 0 && velocity.x == 0)) {
                position.x += penetrationX;
                velocity.x = 0;
                collidedLeft = true;
                
                if (entityType == PLAYER && object->aiType == OCTOROK){
                    this->position = this->startPosition;
                    lives--;
                    hurt = true;
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
    glm::vec3 top_left = glm::vec3(position.x - (width / 2) + 0.05, position.y + (height / 2), position.z);
    glm::vec3 top_right = glm::vec3(position.x + (width / 2) - 0.05, position.y + (height / 2), position.z);
    glm::vec3 bottom = glm::vec3(position.x, position.y - (height / 2), position.z);
    glm::vec3 bottom_left = glm::vec3(position.x - (width / 2) + 0.05, position.y - (height / 2), position.z);
    glm::vec3 bottom_right = glm::vec3(position.x + (width / 2) - 0.05, position.y - (height / 2), position.z);
    
    bool enemy;
    if (entityType == ENEMY) enemy = true;
    else enemy = false;
    
    float penetration_x = 0;
    float penetration_y = 0;
    
    if (map->IsSolid(top, &penetration_x, &penetration_y, enemy, this->room) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
    }
    else if (map->IsSolid(top_left, &penetration_x, &penetration_y, enemy, this->room) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
    }
    else if (map->IsSolid(top_right, &penetration_x, &penetration_y, enemy, this->room) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
    }
    if (map->IsSolid(bottom, &penetration_x, &penetration_y, enemy, this->room) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
    }
    else if (map->IsSolid(bottom_left, &penetration_x, &penetration_y, enemy, this->room) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
    }
    else if (map->IsSolid(bottom_right, &penetration_x, &penetration_y, enemy, this->room) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
    }
}

void Entity::CheckCollisionsX(Map *map)
{
    // Probes for tiles
    glm::vec3 left = glm::vec3(position.x - (width / 2), position.y, position.z);
    glm::vec3 top_left = glm::vec3(position.x - (width /2), position.y + (height / 2) - 0.05, position.z);
    glm::vec3 bottom_left = glm::vec3(position.x - (width / 2), position.y - (height / 2) + 0.05, position.z);
    
    glm::vec3 right = glm::vec3(position.x + (width / 2), position.y, position.z);
    glm::vec3 top_right = glm::vec3(position.x + (width / 2), position.y + (height / 2) - 0.05, position.z);
    glm::vec3 bottom_right = glm::vec3(position.x + (width / 2), position.y - (height / 2) + 0.05, position.z);
    
    bool enemy;
    if (entityType == ENEMY) enemy = true;
    else enemy = false;
    
    float penetration_x = 0;
    float penetration_y = 0;
    if (map->IsSolid(left, &penetration_x, &penetration_y, enemy, this->room) && velocity.x < 0) {
        position.x += penetration_x;
        velocity.x = 0;
        collidedLeft = true;
    }
    else if (map->IsSolid(top_left, &penetration_x, &penetration_y, enemy, this->room) && velocity.x < 0){
        position.x += penetration_x;
        velocity.x = 0;
        collidedLeft = true;
    }
    else if (map->IsSolid(bottom_left, &penetration_x, &penetration_y, enemy, this->room) && velocity.x < 0){
        position.x += penetration_x;
        velocity.x = 0;
        collidedLeft = true;
    }
    
    if (map->IsSolid(right, &penetration_x, &penetration_y, enemy, this->room) && velocity.x > 0) {
        position.x -= penetration_x;
        velocity.x = 0;
        collidedRight = true;
    }
    else if (map->IsSolid(top_right, &penetration_x, &penetration_y, enemy, this->room) && velocity.x > 0) {
        position.x -= penetration_x;
        velocity.x = 0;
        collidedRight = true;
    }
    else if (map->IsSolid(bottom_right, &penetration_x, &penetration_y, enemy, this->room) && velocity.x > 0) {
        position.x -= penetration_x;
        velocity.x = 0;
        collidedRight = true;
    }
    
}
