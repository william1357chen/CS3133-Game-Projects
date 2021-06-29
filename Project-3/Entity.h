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

enum EntityType {Player, Wall, Platform, Text};

class Entity {
public:
    glm::vec3 position;
    glm::vec3 acceleration;
    glm::vec3 velocity;
    float rotation; 

    EntityType entityType;

    float width = 1;
    float height = 1;

    GLuint textureID;
    
    glm::mat4 modelMatrix;

    bool isActive = true;
    
    // Status flag for game level. True means mission successful, and vice versa
    bool result = true;

    bool collidedWall = false;
    bool collidedPlatform = false;

    // Variable for text
    std::string text;

    Entity();

    bool CheckCollision(Entity* others, int otherCount);
    void Update(float deltaTime, Entity* walls = NULL, int wallCount = 0,
        Entity* platforms = NULL, int platformCount = 0);
    void Render(ShaderProgram *program);
    void DrawText(ShaderProgram* program, GLuint fontTextureID, std::string text,
        float size, float spacing, glm::vec3 position);
};
