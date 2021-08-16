#include <iostream>
#include <ctime>
#include "MinMaxTree.h"

//--- Static Variables ---//
//std::unordered_map<std::string, int> MinMaxTree::transpositionTable = std::unordered_map<std::string, int>();



//--- Constructors and Destructor ---//
MinMaxTree::MinMaxTree()
{

}

MinMaxTree::~MinMaxTree()
{

}



//--- Methods ---//
void MinMaxTree::Init(bool _aiIsX, BoardConfiguration _rootConfiguration, bool _startMax)
{
	// Reserve space in the node table to prevent rehashing and speed it up
	// If the AI moves first, there are more possibilities so it needs to reserve more
	if (_aiIsX)
		nodeTable.reserve(6000);
	else
		nodeTable.reserve(2000);
	
	// Going to time how long it takes to create the tree
	auto startTime = time(nullptr);

	// The nodes need to be able to reference the tree later when adding to the node list
	MinMaxNode::tree = this;

	// If the root node has already been created before, we might need to rebuild the tree
	// This means we need to clean up the existing tree first
	if (rootNode != nullptr)
	{
		// If the new root configuration is the same though, we can just go back to the beginning and not regenerate the full tree
		if (_rootConfiguration == rootNode->GetBoardLayout())
		{
			// Go back to the root node but don't rebuild
			currentNode = rootNode;
			return;
		}
		else
		{
			// Since we do have to rebuild the full tree, we need to cleanup the existing one first
			Cleanup();
		}
	}

	// Create the root node. This will start the chain reaction of all of the child nodes getting created
	rootNode = new MinMaxNode(_startMax, _aiIsX, _rootConfiguration);

	// We are starting at the root node
	currentNode = rootNode;

	// Output stats about the tree creation
	auto endTime = time(nullptr);
	std::cout << "\n\n";
	std::cout << "Start Time: " << startTime << std::endl;
	std::cout << "End Time: " << endTime << std::endl;
	std::cout << "Time taken: " << endTime - startTime << std::endl;
	std::cout << "Node List Size: " << nodeTable.size() << std::endl;
}

void MinMaxTree::HandlePlayerMove(BoardConfiguration _newLayout)
{
	// Move down the tree to the node that matches the new board configuration
	currentNode = currentNode->TransitionToLayout(_newLayout);
}

BoardConfiguration MinMaxTree::DecideNextMove()
{
	// Get the new current node after the tree has decided where to move to
	currentNode = currentNode->MakeDecision();

	// Return the board layout at the new current node
	return currentNode->GetBoardLayout();
}

void MinMaxTree::Cleanup()
{
	// Delete all of the nodes in the table and clear up the memory
	for (auto it = nodeTable.begin(); it != nodeTable.end(); it++)
		delete it->second;

	// Reset the node list for later
	nodeTable.clear();
}