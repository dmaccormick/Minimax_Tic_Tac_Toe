#include <Windows.h>
#include <SOIL.h>
#include <GLM/glm.hpp>
#include "TicTacToeBoard.h"

//--- Constructors and Destructor ---//
TicTacToeBoard::TicTacToeBoard()
{
	// Calculate all of the board positions
	for (int i = 0; i < BoardLocation::Num_Locations; i++)
	{
		// Positions start at the top left and go left to right, same as the enum
		int row = i / 3;
		int col = i % 3;
		float x = -165.0f + (165.0f * (float)col);
		float y = 165.0f - (165.0f * (float)row);

		// Store the position
		tilePositions[i] = glm::vec2(x, y);
	}
}

TicTacToeBoard::~TicTacToeBoard()
{
}



//--- Methods ---//
void TicTacToeBoard::Init()
{
	// Load the board texture
	tex_Board = SOIL_load_OGL_texture(
		ASSETS"Images/tex_Board.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);

	// Load the x tile texture
	tex_XTile = SOIL_load_OGL_texture(
		ASSETS"Images/tex_XTile.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);

	// Load the o tile texture
	tex_OTile = SOIL_load_OGL_texture(
		ASSETS"Images/tex_OTile.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);

	// Load the X win message
	tex_XWins = SOIL_load_OGL_texture(
		ASSETS"Images/tex_XWins.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);

	// Load the O win message
	tex_OWins = SOIL_load_OGL_texture(
		ASSETS"Images/tex_OWins.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);

	// Load the tie message
	tex_Tie = SOIL_load_OGL_texture(
		ASSETS"Images/tex_Tie.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);

	// Init the board configuration
	boardLayout.Init();

	// Init the data
	isGameStarted = false;
	isGameOver = false;
	winningTile = '-';
}

void TicTacToeBoard::BeginGame(bool _isPlayerX)
{
	// Reset the board
	boardLayout.Init();

	// The game is now running
	isGameStarted = true;
	isGameOver = false;

	// Determine if the player is X or O
	isPlayerX = _isPlayerX;

	// Reset the winner
	winningTile = '-';
}

void TicTacToeBoard::AddTile(BoardLocation _location, char _newTile)
{
	// Set the tile at the location accordingly
	boardLayout.placedTiles[_location] = _newTile;

	// Now, check if the game is over
	CheckForGameOver();
}

void TicTacToeBoard::Draw(Renderer& renderer)
{
	// If the game is running, draw everything normal colour. Otherwise, mute the colours
	glm::vec3 colour = (isGameStarted && !isGameOver) ? glm::vec3(1.0f) : glm::vec3(0.2f);

	// Draw the board itself
	glm::vec2 boardPosition = glm::vec2(0.0f, 0.0f);
	glm::vec2 boardSize = glm::vec2(512.0f, 512.0f);
	renderer.DrawQuad(boardPosition, boardSize, tex_Board, -1, colour);

	// Draw all of the tiles on top of the board
	for (int i = 0; i < BoardLocation::Num_Locations; i++)
	{
		// Get the tile type from the internally stored board data
		char tileType = boardLayout.placedTiles[i];

		// If the space is empty, just move on
		if (tileType == '-')
			continue;

		// Otherwise, assign the appropriate texture
		GLuint tileTexture = (tileType == 'X') ? tex_XTile : tex_OTile;

		// The tile should be smaller than 1/3rd the size of the board
		glm::vec2 tileSize = glm::vec2(128.0f, 128.0f);

		// Need to position this specific tile according to its location on the board
		glm::vec2 tilePos = tilePositions[i];

		// Draw the tile
		renderer.DrawQuad(tilePos, tileSize, tileTexture, 1, colour);
	}

	// Draw the hover indicator
	if (hoveredTile != BoardLocation::Num_Locations)
	{
		// Since only the player can hover, we use which tile type they are to determine what texture to use
		GLuint hoverTexture = (isPlayerX) ? tex_XTile : tex_OTile;
		glm::vec3 hoverColour = glm::vec3(0.5f);
		glm::vec2 hoverSize = glm::vec2(64.0f, 64.0f);

		// Determine the hover position
		glm::vec2 hoverPos = tilePositions[hoveredTile];

		// Draw the hover indicator
		renderer.DrawQuad(hoverPos, hoverSize, hoverTexture, 0, hoverColour);
	}

	// If the game is over, need to draw the winning banner
	if (isGameOver)
	{
		// Assign the winning tile's banner so we can draw it
		// Can either be a tie game, or one of the players won it
		GLuint winnerBanner = (winningTile == '-') ? tex_Tie : (winningTile == 'X') ? tex_XWins : tex_OWins;

		// Draw the banner
		renderer.DrawQuad(glm::vec2(0.0f), glm::vec2(1024.0f, 256.0f), winnerBanner, 3);
	}
}

void TicTacToeBoard::CheckForGameOver()
{
	// Evaluate the current board layout to see if the game is over
	char winner = boardLayout.EvaluateWinner();

	// If the evaluated value is a space, it means the game has not ended yet so we can just return
	if (winner == ' ')
		return;
	
	// Otherwise, the game is over and we need to show the correct banner
	GameOver(winner);
}

void TicTacToeBoard::GameOver(char _winningTile)
{
	// The game is no longer running since its over
	isGameOver = true;

	// Store the winner so we can draw the appropriate winner banner
	winningTile = _winningTile;
}

void TicTacToeBoard::Cleanup()
{
	// Clean up the texture memory
	glDeleteTextures(1, &tex_Board);
	glDeleteTextures(1, &tex_XTile);
	glDeleteTextures(1, &tex_OTile);
}

void TicTacToeBoard::UpdateMouseHover(glm::vec2 _mousePos)
{
	// If the game is not running, no hover is allowed
	if (!isGameStarted || isGameOver)
	{
		// No hover is being represented by BoardLocation::Num_Locations since the other locations are actual spots for tiles
		hoveredTile = BoardLocation::Num_Locations;
		return;
	}

	// If the game is running, we need to check against every tile to see which one the mouse is over
	for (int i = 0; i < BoardLocation::Num_Locations; i++)
	{
		// Determine the position of the tile
		glm::vec2 tilePos = tilePositions[i];

		// Determine the minimum and the maximum of the tile
		glm::vec2 tileExtent = glm::vec2(82.5f, 82.5f);
		glm::vec2 tileMin = tilePos - tileExtent;
		glm::vec2 tileMax = tilePos + tileExtent;

		// Check if the mouse falls into this tile
		if (_mousePos.x >= tileMin.x && _mousePos.x <= tileMax.x && _mousePos.y >= tileMin.y && _mousePos.y <= tileMax.y)
		{
			// We are in this tile. But if the tile is occupied, we don't want to show the hover above it
			hoveredTile = (boardLayout.placedTiles[i] == '-') ? BoardLocation(i) : BoardLocation::Num_Locations;

			// Since we found the tile, we can just return and avoid checking the others
			return;
		}
	}

	// If none of the tiles match, none of them are hovered
	hoveredTile = BoardLocation::Num_Locations;
}

bool TicTacToeBoard::HandleMouseClick()
{
	// If no tile is currently being hovered over, just return false to indicate no tiles were placed
	if (hoveredTile == BoardLocation::Num_Locations)
		return false;

	// Determine the player's tile type since only the player can perform a mouse click
	char playerChar = (isPlayerX) ? 'X' : 'O';

	// Add a new tile at the location
	AddTile(hoveredTile, playerChar);

	// No longer hovering over an empty space
	hoveredTile = BoardLocation::Num_Locations;

	// Return true to indicate a tile was placed
	return true;
}

void TicTacToeBoard::DEBUG_HandleMouseRightClick()
{
	// If no tile is currently being hovered over, just return
	if (hoveredTile == BoardLocation::Num_Locations)
		return;

	// Determine the player's tile type since only the player can perform a mouse click
	// This DEBUG function is supposed to represent the AI player so actually do the opposite!
	// This is needed to determine if the game is over or not
	char playerChar = (!isPlayerX) ? 'X' : 'O';

	// Add a new tile at the location
	AddTile(hoveredTile, playerChar);

	// No longer hovering over an empty space
	hoveredTile = BoardLocation::Num_Locations;
}

void TicTacToeBoard::HandleAIMove(BoardConfiguration _newLayout)
{
	// Store the new board layout
	boardLayout = _newLayout;

	// Check if the game is over now
	CheckForGameOver();
}



//--- Setters and Getters ---//
BoardConfiguration TicTacToeBoard::GetCurrentLayout() const {
	return boardLayout;
}

bool TicTacToeBoard::GetIsGameOver() const {
	return isGameOver;
}