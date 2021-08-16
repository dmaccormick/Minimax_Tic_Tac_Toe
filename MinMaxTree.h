#pragma once

#include <vector>
#include <unordered_map>
#include "BoardConfiguration.h"

class MinMaxNode;

class MinMaxTree
{
public:
	//--- Constructors and Destructor ---//
	MinMaxTree();
	~MinMaxTree();

	//--- Methods ---//
	void Init(bool _aiIsX, BoardConfiguration _rootConfiguration, bool _startMax = true);
	void HandlePlayerMove(BoardConfiguration _newLayout);
	BoardConfiguration DecideNextMove();
	void Cleanup();

	//--- Public Variables ---//
	std::unordered_map<std::string, MinMaxNode*> nodeTable;

private:
	//--- Data ---//
	MinMaxNode* rootNode;
	MinMaxNode* currentNode;
};

class MinMaxNode
{
public:
	//--- Constructors and Destructor ---//
	MinMaxNode(bool _isMaxNode, bool _aiIsX, BoardConfiguration _boardLayout);
	~MinMaxNode();

	//--- Methods ---//
	MinMaxNode* TransitionToLayout(BoardConfiguration _boardLayout);
	MinMaxNode* MakeDecision();

	//--- Setters and Getters ---//
	int GetNodeScore() const;
	BoardConfiguration GetBoardLayout() const;

	//--- Static Variables ---//
	static MinMaxTree* tree;

private:
	//--- Data ---//
	std::vector<MinMaxNode*> children;
	bool isLeafNode;
	int nodeScore;
	bool isMaxNode;
	BoardConfiguration boardLayout;

	//--- Uility Functions ---//
	std::vector<BoardLocation> FindEmptySpaces();
	BoardConfiguration FillEmptySpace(BoardConfiguration _boardLayout, BoardLocation _emptySpace, char _aiTileType);
	void DetermineLeafScore(char _aiTile);
	void DetermineBranchScore();
};