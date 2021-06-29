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

#include "Entity.h"

#ifndef PLAYER_H
#define PLAYER_H
class Player : public Entity {
public:
	int* animeRight = nullptr;
	int* animeLeft = nullptr;
	int* animeUp = nullptr;
	int* animeDown = nullptr;

	int* animeIndices = nullptr;

	int animeFrames = 0;
	int animeIndex = 0;
	float animeTime = 0;
	int animeCols = 0;
	int animeRows = 0;

	virtual void Update(float deltaTime);

	virtual void Render(ShaderProgram* program);
};

#endif