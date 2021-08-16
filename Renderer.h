#pragma once

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>

class Renderer
{
public:
	//--- Constructors and Destructor ---//
	Renderer();
	~Renderer();

	//--- Methods ---//
	void Init();
	void PreRender();
	void PostRender();
	void DrawQuad(glm::vec2 _position, glm::vec2 _size, GLuint _texture, int _zOrder = 0, glm::vec3 _colour = glm::vec3(1.0f));
	void Cleanup();

private:
	//--- Data ---//
	GLuint mvp_loc, col_loc;
	GLuint shader_program;
	GLuint quad_vbo; // vertex buffer object
	GLuint quad_vao; // vertex array object
	glm::mat4 modelMatrix;
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;
	bool isInit;
};