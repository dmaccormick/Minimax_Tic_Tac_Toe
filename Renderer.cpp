#include "Renderer.h"
#include "Shaders.h"

//--- Constructors and Destructor ---//
Renderer::Renderer()
{

}

Renderer::~Renderer()
{

}



//--- Methods ---//
void Renderer::Init()
{
	// Ensure this is only init once
	if (isInit)
		return;

	// Below is the original Initialize() code provided by David, originally in main.cpp
	// Create a shader for the assignment
	GLuint vs = buildShader(GL_VERTEX_SHADER, ASSETS"primitive.vs");
	GLuint fs = buildShader(GL_FRAGMENT_SHADER, ASSETS"primitive.fs");
	shader_program = buildProgram(vs, fs, 0);
	dumpProgram(shader_program, "Assignment 3 shader program");

	// Create all 4 vertices of the quad
	glm::vec3 p0 = glm::vec3(-1.0f, -1.0f, 0.0f);
	glm::vec3 p1 = glm::vec3(-1.0f, 1.0f, 0.0f);
	glm::vec3 p2 = glm::vec3(1.0f, -1.0f, 0.0f);
	glm::vec3 p3 = glm::vec3(1.0f, 1.0f, 0.0f);

	// Create a list of vertices
	glm::vec3 vertices[12] =
	{
		// Bottom face
		p0, p1, p2, p3,
	};

	glGenVertexArrays(1, &quad_vao);
	glBindVertexArray(quad_vao);

	glGenBuffers(1, &quad_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

	glUseProgram(shader_program);
	GLuint vPosition = glGetAttribLocation(shader_program, "vPosition");
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);

	mvp_loc = glGetUniformLocation(shader_program, "modelViewProjMat");
	col_loc = glGetUniformLocation(shader_program, "colour");

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// This is now set up
	isInit = true;
}

void Renderer::PreRender()
{
	// Below is the original Render() code provided by David, originally in main.cpp
	glUseProgram(shader_program);

	viewMatrix = glm::mat4(1.0f);
	projectionMatrix = glm::ortho(-640.0f, 640.0f, -360.0f, 360.0f, -10.0f, 10.0f);

	glActiveTexture(GL_TEXTURE0);
}

void Renderer::DrawQuad(glm::vec2 _position, glm::vec2 _size, GLuint _texture, int _zOrder, glm::vec3 _colour)
{
	// If the renderer hasn't been init for some reason, make sure to do that
	if (!isInit)
		Init();

	// Bind the requested texture
	glBindTexture(GL_TEXTURE_2D, _texture);

	// Below is the original DrawQuad() code provided by David which was in main.cpp before
	modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(_position.x, _position.y, (float)_zOrder));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(_size.x * 0.5f, _size.y * 0.5f, 1.0f));

	glm::mat4 modelViewProjMat = projectionMatrix * viewMatrix * modelMatrix;

	glUniformMatrix4fv(mvp_loc, 1, 0, &modelViewProjMat[0][0]);
	glUniform3fv(col_loc, 1, &_colour[0]);

	glBindVertexArray(quad_vao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void Renderer::PostRender()
{
	// Below is the end of the drawing code in Render() provided by David, originally in main.cpp
	glUseProgram(GL_NONE);
}

void Renderer::Cleanup()
{
	// Below is the original cleanup code provided by David, originally in main.cpp
	glDeleteBuffers(1, &quad_vbo);
	glDeleteVertexArrays(1, &quad_vao);
	glDeleteProgram(shader_program);
}