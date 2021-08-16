#include "BoardConfiguration.h"

//--- Methods ---//
void BoardConfiguration::Init()
{
	// Set all of the spaces to neutral by default
	/*for (int i = 0; i < BoardLocation::Num_Locations; i++)
		placedTiles[i] = '-';*/
	placedTiles = "---------";
}

char BoardConfiguration::EvaluateWinner()
{
	// Check each of the 8 possible win states (matched row x3, matched col x3, diagonal x2)
	// Order below is: top row, middle row, bottom row, left col, middle col, right col, top left -> bottom right diagonal, top right -> bottom left diagonal
	// Basically checking if they all match AND they're not all neutral (they match if they're neutral so need to check this too)
	// Rows
	if (placedTiles[Top_Left] != '-' && placedTiles[Top_Left] == placedTiles[Top_Middle] && placedTiles[Top_Left] == placedTiles[Top_Right])
		return placedTiles[Top_Left];
	else if (placedTiles[Center_Left] != '-' && placedTiles[Center_Left] == placedTiles[Center_Middle] && placedTiles[Center_Left] == placedTiles[Center_Right])
		return placedTiles[Center_Left];
	else if (placedTiles[Bottom_Left] != '-' && placedTiles[Bottom_Left] == placedTiles[Bottom_Middle] && placedTiles[Bottom_Left] == placedTiles[Bottom_Right])
		return placedTiles[Bottom_Left];

	// Cols
	else if (placedTiles[Top_Left] != '-' && placedTiles[Top_Left] == placedTiles[Center_Left] && placedTiles[Top_Left] == placedTiles[Bottom_Left])
		return placedTiles[Top_Left];
	else if (placedTiles[Top_Middle] != '-' && placedTiles[Top_Middle] == placedTiles[Center_Middle] && placedTiles[Top_Middle] == placedTiles[Bottom_Middle])
		return placedTiles[Top_Middle];
	else if (placedTiles[Top_Right] != '-' && placedTiles[Top_Right] == placedTiles[Center_Right] && placedTiles[Top_Right] == placedTiles[Bottom_Right])
		return placedTiles[Top_Right];

	// Diagonals
	else if (placedTiles[Top_Left] != '-' && placedTiles[Top_Left] == placedTiles[Center_Middle] && placedTiles[Top_Left] == placedTiles[Bottom_Right])
		return placedTiles[Top_Left];
	else if (placedTiles[Top_Right] != '-' && placedTiles[Top_Right] == placedTiles[Center_Middle] && placedTiles[Top_Right] == placedTiles[Bottom_Left])
		return placedTiles[Top_Right];


	// If none of the win states triggered, we should show a tie message instead
	// Loop through all of the tiles and check if every single one is filled. If it is, show the tie message
	for (int i = 0; i < BoardLocation::Num_Locations; i++)
	{
		// If this tile is empty, the game is not over yet so we can safely return out
		// Using space to represent this
		if (placedTiles[i] == '-')
			return ' ';
	}

	// If we made it here, then all of the tile locations have been filled and none of them triggered the win message. Therefore, we must have a tie
	return '-';
}

bool BoardConfiguration::operator==(const BoardConfiguration& other) 
{
	// Check if all of the placed tiles match eachother
	return (placedTiles[0] == other.placedTiles[0] && placedTiles[1] == other.placedTiles[1] && placedTiles[2] == other.placedTiles[2] &&
		placedTiles[3] == other.placedTiles[3] && placedTiles[4] == other.placedTiles[4] && placedTiles[5] == other.placedTiles[5] &&
		placedTiles[6] == other.placedTiles[6] && placedTiles[7] == other.placedTiles[7] && placedTiles[8] == other.placedTiles[8]);
}