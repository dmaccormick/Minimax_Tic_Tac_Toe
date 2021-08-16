#pragma once

#include <string>
#include <GLM/glm.hpp>

enum BoardLocation
{
	Top_Left,
	Top_Middle,
	Top_Right,

	Center_Left,
	Center_Middle,
	Center_Right,

	Bottom_Left,
	Bottom_Middle,
	Bottom_Right,

	Num_Locations
};

struct BoardConfiguration
{
	//--- Methods ---//
	void Init();
	char EvaluateWinner();
	bool operator==(const BoardConfiguration& other);

	//--- Data ---//
	// In the order as outlined from the enum above [	TL,TM,TR / CL,CM,CR / BL,BM,BR	]
	// Either 'X', 'O', or '-' ('-' is for neutral)
	//char placedTiles[BoardLocation::Num_Locations];
	std::string placedTiles;
};