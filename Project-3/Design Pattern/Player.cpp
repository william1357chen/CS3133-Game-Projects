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

#include "Player.h"

void Player::Update(float deltaTime) {
	Entity::Update(deltaTime);
	if (animeIndices != nullptr) {
		if (glm::length(movement) != 0){
			animeTime += deltaTime;

			if (animeTime >= 0.25f) {
				animeTime = 0.0f;
				animeIndex++;
				if (animeIndex >= animeFrames)
					animeIndex = 0;
			}
		}
		else {
			animeIndex = 0;
		}
	}
}

void Player::Render(ShaderProgram* program) {
	program->SetModelMatrix(modelMatrix);
	if (animeIndices != nullptr) {
		int index = animeIndices[animeIndex];
		float u = (float)(index % animeCols) / (float)animeCols;
		float v = (float)(index / animeCols) / (float)animeRows;
		float width = 1.0f / (float)animeCols;
		float height = 1.0f / (float)animeRows;
		float texCoords[] = { u, v + height, u + width, v + height, u + width, v,
		u, v + height, u + width, v, u, v };
		float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
		glBindTexture(GL_TEXTURE_2D, textureID);
		glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
		glEnableVertexAttribArray(program->positionAttribute);
		glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
		glEnableVertexAttribArray(program->texCoordAttribute);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(program->positionAttribute);
		glDisableVertexAttribArray(program->texCoordAttribute);
	}
	else {
		Entity::Render(program);
	}
	
}
