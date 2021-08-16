#pragma once

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include "Renderer.h"
#include "BoardConfiguration.h"

class TicTacToeBoard
{
public:
	//--- Constructors and Destructor ---//
	TicTacToeBoard();
	~TicTacToeBoard();

	//--- Methods ---//
	void Init();
	void BeginGame(bool _playerIsX);
	void AddTile(BoardLocation _location, char _newTile);
	void Draw(Renderer& _renderer);
	void CheckForGameOver();
	void GameOver(char _winningTile);
	void Cleanup();
	void UpdateMouseHover(glm::vec2 _mousePos);
	bool HandleMouseClick();
	void DEBUG_HandleMouseRightClick();
	void HandleAIMove(BoardConfiguration _newLayout);

	//--- Setters and Getters ---//
	BoardConfiguration GetCurrentLayout() const;
	bool GetIsGameOver() const;

private:
	//--- Renderables ---//
	GLuint tex_Board;
	GLuint tex_XTile;
	GLuint tex_OTile;
	GLuint tex_XWins;
	GLuint tex_OWins;
	GLuint tex_Tie;
	GLuint tex_ThinkingMessage;

	//--- Data ---//
	glm::vec2 tilePositions[BoardLocation::Num_Locations];
	BoardLocation hoveredTile;
	bool isGameStarted;
	bool isGameOver;
	bool isPlayerX;
	char winningTile;
	BoardConfiguration boardLayout;
};