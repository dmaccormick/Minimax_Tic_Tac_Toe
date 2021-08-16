#include <GL/gl3w.h>
#include <GLFW/glfw3.h> // GLFW helper library
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp> // for glm::ortho

// IMGUI
#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>

#include <iostream> // Used for 'cout'
#include <stdio.h>  // Used for 'printf'
#include <SOIL.h>

// Tic Tac Toe
#include "Renderer.h"
#include "TicTacToeBoard.h"
#include "MinMaxTree.h"

/*---------------------------- Variables ----------------------------*/
// GLFW window
GLFWwindow* window;
int width = 1280;
int height = 720;

// Tic Tac Toe
Renderer renderer;
TicTacToeBoard board;
MinMaxTree tree;
int playerTileChoice; //0 = X, 1 = O
glm::vec2 mousePos;
bool playerFirstMove = true;


void OnMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	// When the left mouse is pressed, we need to tell the board to handle it and maybe place a tile
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		// Try to place a tile on the board. Returns true if the tile placement succeeded
		if (board.HandleMouseClick())
		{
			// If this is the first move made in the game, we need to init the AI now
			if (playerFirstMove)
			{
				// Init the AI tree now that the player has placed the first tile. Use the board's configuration as the root node
				tree.Init(false, board.GetCurrentLayout());

				// Now, the AI needs to respond
				board.HandleAIMove(tree.DecideNextMove());

				// No longer the first move
				playerFirstMove = false;
			}
			else
			{
				// Only do the AI move if the game isn't over
				if (!board.GetIsGameOver())
				{
					// We need to transition to the next tree node based on the player's choice
					tree.HandlePlayerMove(board.GetCurrentLayout());

					// Now, the AI needs to respond
					board.HandleAIMove(tree.DecideNextMove());
				}
			}
		}
	}
	
	//// DEBUG: Can place tiles for the AI using the right click
	//if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	//	board.DEBUG_HandleMouseRightClick();
}

void Initialize()
{
	// Set up a callback for mouse buttons within GLFW
	glfwSetMouseButtonCallback(window, OnMouseButtonCallback);

	// Init the TicTacToe Game
	renderer.Init();
	board.Init();
}

float Lerp(float _a, float _b, float _t) {
	return ((1.0f - _t) * _a) + (_t * _b);
}

void CalculateMousePos()
{
	// Get the mouse position from GLFW in window space (0,0) is top left
	double mouseRawX = 0.0f;
	double mouseRawY = 0.0f;
	glfwGetCursorPos(window, &mouseRawX, &mouseRawY);
	
	// Convert the x-coord into game space (-640->640)
	float halfWidth = width / 2.0f;
	float mouseX = 0.0f;
	if (mouseRawX < halfWidth)
		mouseX = Lerp(-halfWidth, 0.0f, mouseRawX / halfWidth);
	else
		mouseX = Lerp(0.0f, halfWidth, (mouseRawX - halfWidth) / halfWidth);

	// Convert the x-coord into game space (-360->360)
	float halfHeight = height / 2.0f;
	float mouseY = 0.0f;
	if (mouseRawY < halfHeight)
		mouseY = Lerp(halfHeight, 0.0f, mouseRawY / halfHeight);
	else
		mouseY = Lerp(0.0f, -halfHeight, (mouseRawY - halfHeight) / halfHeight);

	// Store the mouse position
	mousePos = glm::vec2(mouseX, mouseY);
}

void Update(float a_deltaTime)
{
	// Exit by pressing escape
    if (glfwGetKey(window, GLFW_KEY_ESCAPE))
        glfwSetWindowShouldClose(window, true);

	// Calculate the updated mouse position
	CalculateMousePos();

	// Update the mouse hover over the tiles
	board.UpdateMouseHover(mousePos);
}

void Render()
{
	// Set up to render the frame
	renderer.PreRender();

	// Draw calls for the frame
	board.Draw(renderer);

	// Finish rendering the frame
	renderer.PostRender();
}

void GUI()
{
    ImGui::Begin("Settings", 0, ImVec2(250, 100), 0.4f);
    {
        // Show some basic stats in the settings window 
        ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Spacing();
		ImGui::Spacing();

		// Show the game settings
		ImGui::Text("Choose Your Tile Type...");
		ImGui::RadioButton("X", &playerTileChoice, 0); ImGui::SameLine();
		ImGui::RadioButton("O", &playerTileChoice, 1);

		// Start the game when the button is pressed
		if (ImGui::Button("Begin Game", ImVec2(200.0f, 30.0f)))
		{
			// Decide if the player is making the first move
			playerFirstMove = (playerTileChoice == 0);

			// Start the game
			board.BeginGame(playerTileChoice == 0);

			// If the AI is making the first move (ie: player chose O), we need to create the tree now
			if (!playerFirstMove)
			{
				// Create a blank board layout to use as the base for the AI
				BoardConfiguration blankLayout = BoardConfiguration();
				blankLayout.Init();

				// Set up the AI tree now
				tree.Init(true, blankLayout);

				//// Set up the AI tree with a test layout
				//BoardConfiguration testLayout = BoardConfiguration();
				//testLayout.placedTiles[0] = 'X'; testLayout.placedTiles[1] = 'O'; testLayout.placedTiles[2] = 'O';
				//testLayout.placedTiles[3] = '-'; testLayout.placedTiles[4] = 'X'; testLayout.placedTiles[5] = '-';
				//testLayout.placedTiles[6] = '-'; testLayout.placedTiles[7] = 'X'; testLayout.placedTiles[8] = 'O';
				//tree.Init(true, testLayout, true);

				// Make a decision to start the game
				board.HandleAIMove(tree.DecideNextMove());
			}
		}
    }
    ImGui::End();
}

void Cleanup()
{
	renderer.Cleanup();
	board.Cleanup();
	tree.Cleanup();
}

static void ResizeEvent(GLFWwindow* a_window, int a_width, int a_height)
{
    // Set the viewport incase the window size changed
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
}

int main()
{
    // start GL context and O/S window using the GLFW helper library
    if (!glfwInit())
    {
        fprintf(stderr, "ERROR: could not start GLFW3\n");
        return 1;
    }

    window = glfwCreateWindow(width, height, "Assignment 3", NULL, NULL);
    if (!window)
    {
        fprintf(stderr, "ERROR: could not open window with GLFW3\n");
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSetWindowSizeCallback(window, ResizeEvent);
    glfwSwapInterval(0);

    // start GL3W
    gl3wInit();

    // Setup ImGui binding. This is for any parameters you want to control in runtime
    ImGui_ImplGlfwGL3_Init(window, true);
    ImGui::StyleColorsLight();

    // Get version info
    const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
    const GLubyte* version = glGetString(GL_VERSION); // version as a string
    printf("Renderer: %s\n", renderer);
    printf("OpenGL version supported %s\n", version);

    // tell GL to only draw onto a pixel if the shape is closer to the viewer
    glEnable(GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"

    Initialize();

    float oldTime = 0.0f;
    while (!glfwWindowShouldClose(window))
    {
        float currentTime = (float)glfwGetTime();
        float deltaTime = currentTime - oldTime;
        oldTime = currentTime;

        // update other events like input handling 
        glfwPollEvents();

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ImGui_ImplGlfwGL3_NewFrame();

        // Call the helper functions
        Update(deltaTime);
        Render();
        GUI();

        // Finish by drawing the GUI on top of everything
        ImGui::Render();
        glfwSwapBuffers(window);
    }

    // close GL context and any other GLFW resources
    glfwTerminate();
    ImGui_ImplGlfwGL3_Shutdown();
    Cleanup();
    return 0;
}