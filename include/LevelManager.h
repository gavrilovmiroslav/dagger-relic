#pragma once

#include "Prelude.h"
#include <fstream>
#include <iostream>

#define TILE_SIZE 64
#define TILE_ROWS 10
#define TILE_COLS 10
#define HEIGHT = ILE_SIZE * TILE_ROWS
#define WIDTH = ILE_SIZE * TTILE_COLS 

class LevelManager
{
public:
	void LoadLevel(String file);
	char levelMap[TILE_ROWS][TILE_COLS] = { '0' };

private:
	std::ifstream levelFile;
};
