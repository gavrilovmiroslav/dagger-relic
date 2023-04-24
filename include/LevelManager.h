#pragma once

#include <fstream>
#include "Prelude.h"

#define TILE_ROWS 10
#define TILE_COLS 10

class LevelManager
{
public:
	void load_level(String file);
	Char level_map[TILE_ROWS][TILE_COLS] = { '0' };

private:
	std::ifstream level_file;
};
