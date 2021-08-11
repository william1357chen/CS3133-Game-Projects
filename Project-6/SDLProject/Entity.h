#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Map.h"
#include <string>

enum EntityType {PLAYER, ENEMY, PLATFORM, TEXT, WEAPON};
enum AIType {OCTOROK, WIZARD, FIRE, ROOM, SWORD};
enum AIState {IDLE, WALKING, FOLLOWING};

class Entity {
public:

    EntityType entityType;
    AIType aiType;
    AIState aiState;
    
    Entity* weapon = NULL;
    bool armed = false;
    bool attacking = false;
    bool visited = false;
    
    glm::vec3 startPosition;
    
    std::string text;
    bool complete = false;
    bool room = false;
    int lives;
    
    bool hurt = false;
    
    glm::vec3 position;
    glm::vec3 velocity;

    float width = 1;
    float height = 1;

    GLuint textureID;
    
    glm::mat4 modelMatrix;
    
    int *animRight = NULL;
    int *animLeft = NULL;
    int *animUp = NULL;
    int *animDown = NULL;
    
    // attack
    int *animAttackRight = NULL;
    int *animAttackLeft = NULL;
    int *animAttackUp = NULL;
    int *animAttackDown = NULL;

    int *animIndices = NULL;
    int animFrames = 0;
    int animIndex = 0;
    float animTime = 0;
    int animCols = 0;
    int animRows = 0;

    bool isActive = true;

    bool collidedTop = false;
    bool collidedBottom = false;
    bool collidedLeft = false;
    bool collidedRight = false;
    
    Entity();
    void CheckCollisionsY(Entity* objects, int objectCount);
    void CheckCollisionsX(Entity* objects, int objectCount);
    void CheckCollisionsX(Map *map);
    void CheckCollisionsY(Map *map);

    bool CheckCollision(Entity* other);
    void Update(float deltaTime, Entity* player, Entity* enemies, int enemyCount, Map* map);
    void Render(ShaderProgram *program);
    void DrawSpriteFromTextureAtlas(ShaderProgram *program, GLuint textureID, int index);
    void DrawText(ShaderProgram* program, GLuint fontTextureID, std::string text,
             float size, float spacing, glm::vec3 position);
    
    void Player(float deltaTime, Entity* enemies, int enemyCount, Map* map);

    void AI(float deltaTime, Entity* player, Entity* enemies, int enemyCount, Map* map);
    void ChangeDirection();
    
    void Weapon(float deltaTime, Entity* player, Entity* enemies, int enemyCount, Map* map);
};
