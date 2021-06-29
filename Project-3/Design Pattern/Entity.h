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

#ifndef ENTITY_H
#define ENTITY_H
class Entity {
public:
	glm::vec3 position;
	glm::vec3 movement;
	glm::vec3 speed;
	GLuint textureID;
	glm::mat4 modelMatrix;
	Entity();
	virtual void Update(float deltaTime);
	virtual void Render(ShaderProgram* program);
};
#endif