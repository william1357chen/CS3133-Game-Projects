#include "Entity.h"
#include <string>
#include <vector>

Entity::Entity()
{
    position = glm::vec3(0);
    acceleration = glm::vec3(0);
    velocity = glm::vec3(0);
    rotation = 0.0f;
    modelMatrix = glm::mat4(1.0f);
}

void Entity::Update(float deltaTime, Entity* walls, int wallCount, 
    Entity* platforms, int platformCount)
{   
    if (isActive == false) return;

    if (entityType == Player) {
        velocity += acceleration * deltaTime;
        position += velocity * deltaTime; // Move on Y
        if (CheckCollision(walls, wallCount)) {
            collidedWall = true;
        }
        if (CheckCollision(platforms, platformCount)) {
            collidedPlatform = true;
        }
        
        if (collidedPlatform == true) {
            result = true;
            isActive = false;
        }
        else if (collidedWall == true) {
            result = false;
            isActive = false;
        }
        modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, position);
    }
    if (entityType == Wall) {
        if (rotation >= 360.0f)
            rotation = 0.0f;
        rotation += 45.0 * deltaTime;
        modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, position);
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
    }
    if (entityType == Platform) {
        modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, position);
    }
    
}

bool Entity::CheckCollision(Entity* others, int otherCount) {

    for (int i = 0; i < otherCount; i++) {
        Entity* other = &others[i];
        float xdist = fabs(position.x - other->position.x) - ((width + other->width) / 2.0f);
        float ydist = fabs(position.y - other->position.y) - ((height + other->height) / 2.0f);
        if (xdist < 0 && ydist < 0) {
            return true;
        }
    }
    return false;
}

void Entity::Render(ShaderProgram *program) {

    if (entityType != Text) {
        program->SetModelMatrix(modelMatrix);

        float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
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
    else { // entityType == Text
        if (isActive == false) {return; }
        DrawText(program, textureID, text, 0.75f, -0.25f, position);
    }

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