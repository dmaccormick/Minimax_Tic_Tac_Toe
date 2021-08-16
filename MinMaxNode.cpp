#include <algorithm>
#include <iostream>
#include "MinMaxTree.h"

//--- Static Variables ---//
MinMaxTree* MinMaxNode::tree = nullptr;



//--- Constructors and Destructor ---//
MinMaxNode::MinMaxNode(bool _isMaxNode, bool _aiIsX, BoardConfiguration _boardLayout)
{
	// Store this node's data
	isMaxNode = _isMaxNode;
	boardLayout = _boardLayout;
	nodeScore = (isMaxNode) ? -10 : 10;

	// Add this node to the node list
	tree->nodeTable.insert(std::pair<std::string, MinMaxNode*>(_boardLayout.placedTiles, this));

	// Determine if the AI is X or O
	char aiTileType = (_aiIsX) ? 'X' : 'O';

	// Determine the empty spaces that can be filled in the layout
	std::vector<BoardLocation> emptySpaces = FindEmptySpaces();

	// If there are no empty spaces, this is a leaf node. We need to determine the score of this node based on if this is a win or a loss
	// Alternatively, if the game is over, it is also a leaf node (the game can end in as few as 5 moves)
	if (boardLayout.EvaluateWinner() != ' ' || emptySpaces.size() == 0)
	{
		isLeafNode = true;
		DetermineLeafScore(aiTileType);
		return;
	}
	else
		isLeafNode = false;

	// If there are empty spaces, create child nodes for each of them
	for (int i = 0; i < emptySpaces.size(); i++)
	{
		// Create a new board layout with the empty space filled in with whatever the next row turn would be
		char tileToAddToChild = '-';
		if (_aiIsX)
		{
			if (isMaxNode)
				tileToAddToChild = 'X';
			else
				tileToAddToChild = 'O';
		}
		else
		{
			if (isMaxNode)
				tileToAddToChild = 'O';
			else
				tileToAddToChild = 'X';
		}
		BoardConfiguration childLayout = FillEmptySpace(boardLayout, emptySpaces[i], tileToAddToChild);

		// Check if the child node layout already exists in the list. If so, just merge and use that node instead
		auto childNode = tree->nodeTable.find(childLayout.placedTiles);
		if (childNode == tree->nodeTable.end())
		{
			// Create a new node and assign it the layout
			// MinMax trees flip min-max so assign it the opposite of this node
			children.push_back(new MinMaxNode(!isMaxNode, _aiIsX, childLayout));
		}
		else
		{
			// If a node with the layout is already cached, just use it instead
			children.push_back(childNode->second);
		}

		// Get the child score for alpha-beta pruning
		int childScore = children[children.size() - 1]->GetNodeScore();

		// Assign the new best score if need be
		nodeScore = (_isMaxNode) ? std::max(nodeScore, childScore) : std::min(nodeScore, childScore);
	}
}

MinMaxNode::~MinMaxNode()
{
	// Cleanup is handled within MinMaxTree::Cleanup()
}



//--- Methods ---//
MinMaxNode* MinMaxNode::TransitionToLayout(BoardConfiguration _boardLayout)
{
	// Loop through the children and find the one that matches the given board layout. That's the new current node
	for (int i = 0; i < children.size(); i++)
	{
		// If the board layout matches, return the child
		if (children[i]->boardLayout == _boardLayout)
			return children[i];
	}
}

MinMaxNode* MinMaxNode::MakeDecision()
{
	if (isLeafNode)
		return this;

	// If this is a min node, the 'best' score is the lowest, otherwise it is the highest
	std::vector<MinMaxNode*> goodOptions = std::vector<MinMaxNode*>();
	goodOptions.push_back(children[0]);
	int bestScore = children[0]->GetNodeScore();
	int bestIndex = 0;
	for (int i = 1; i < children.size(); i++)
	{
		// Get the score from the node
		int childScore = children[i]->GetNodeScore();

		// Determine if it is better than the currently stored best score
		// If this is a max node, 'better' is higher. For min nodes, 'better' is lower
		if (isMaxNode)
		{
			if (childScore > bestScore)
			{
				bestScore = childScore;
				bestIndex = i;

				//reset the good options list since we have a new best score
				goodOptions.clear();
				goodOptions.push_back(children[i]);
			}
			else if (childScore == bestScore)
				goodOptions.push_back(children[i]);
		}
		else
		{
			if (childScore < bestScore)
			{
				bestScore = childScore;
				bestIndex = i;

				//reset the good options list since we have a new best score
				goodOptions.clear();
				goodOptions.push_back(children[i]);
			}
			else if (childScore == bestScore)
				goodOptions.push_back(children[i]);
		}
	}

	// Randomly select one of the good children
	int index = rand() % goodOptions.size();
	return goodOptions[index];
}



//--- Setters and Getters ---//
int MinMaxNode::GetNodeScore() const {
	return nodeScore;
}

BoardConfiguration MinMaxNode::GetBoardLayout() const {
	return boardLayout;
}



//--- Utility Functions ---//
std::vector<BoardLocation> MinMaxNode::FindEmptySpaces()
{
	// Create a vector to hold the positions
	std::vector<BoardLocation> emptySpaces = std::vector<BoardLocation>();

	// Loop through the board layout and find the spaces
	for (int i = 0; i < BoardLocation::Num_Locations; i++)
	{
		// If the space is empty, add it to the list
		if (boardLayout.placedTiles[i] == '-')
			emptySpaces.push_back(BoardLocation(i));
	}

	// Return the list of empty spaces
	return emptySpaces;
}

BoardConfiguration MinMaxNode::FillEmptySpace(BoardConfiguration _boardLayout, BoardLocation _emptySpace, char _aiTileType)
{
	// Fill in the location on the board layout
	_boardLayout.placedTiles[_emptySpace] = _aiTileType;

	// Return the configuration
	return _boardLayout;
}

void MinMaxNode::DetermineLeafScore(char _aiTileType)
{
	// Otherwise, we need to determine the winner for this board layout
	char winner = boardLayout.EvaluateWinner();

	// Assign a point value based on the winner
	// Winner is AI = 1, winner is player = -1, game ties = 0
	nodeScore = (winner == '-') ? 0 : (winner == _aiTileType) ? 1 : -1;
}

void MinMaxNode::DetermineBranchScore()
{
	// Assign the best score to be the first child by default
	// There should always be at least one child since this is not a leaf node
	int bestScore = children[0]->GetNodeScore();

	// Loop through the rest of the children and see if there is a better score
	// 'Better' means either less or greater, depending on if this is a max or min node
	for (int i = 1; i < children.size(); i++)
	{
		// Get the score from the node
		int childScore = children[i]->GetNodeScore();

		// Determine if it is better than the currently stored best score
		// If this is a max node, 'better' is higher. For min nodes, 'better' is lower
		bestScore = (isMaxNode) ? std::max(bestScore, childScore) : std::min(bestScore, childScore);
	}

	// Assign the best score to this node
	nodeScore = bestScore;
}