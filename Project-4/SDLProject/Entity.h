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
#include <string>
enum EntityType {PLAYER, ENEMY, PLATFORM, TEXT};
enum AIType {GOOMBA, KOOPA_TROOPA, KOOPA_PARATROOPA};
enum AIState {IDLE, WALKING, FOLLOWING, FLYING};

class Entity {
public:

    EntityType entityType;
    AIType aiType;
    AIState aiState;

    glm::vec3 position;
    glm::vec3 movement;
    glm::vec3 acceleration;
    glm::vec3 velocity;
    float speed;

    float width = 1;
    float height = 1;

    bool jump = false;
    float jumpPower = 0;

    GLuint textureID;
    
    glm::mat4 modelMatrix;
    
    int *animRight = NULL;
    int *animLeft = NULL;
    int *animUp = NULL;
    int *animDown = NULL;
    int *animJumpLeft = NULL;
    int *animJumpRight = NULL;
    int *animDead = NULL;

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
    
    std::string text;
    
    Entity();
    void CheckCollisionsY(Entity* objects, int objectCount);
    void CheckCollisionsX(Entity* objects, int objectCount);

    bool CheckCollision(Entity* other);
    void Update(float deltaTime, Entity* player, Entity* platforms, int platformCount, Entity* enemies, int enemyCount);
    void Render(ShaderProgram *program);
    void DrawSpriteFromTextureAtlas(ShaderProgram *program, GLuint textureID, int index);
    void DrawText(ShaderProgram* program, GLuint fontTextureID, std::string text,
             float size, float spacing, glm::vec3 position);
    
    void Player(float deltaTime, Entity* platforms, int platformCount, Entity* enemies, int enemyCount);

    void AI(float deltaTime, Entity* player, Entity* platforms, int platformCount);
    void AIGoomba();
    void AIKoopaTroopa(Entity* player);
    void AIKoopaParatroopa();
};
