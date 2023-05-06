#pragma once

#include <deque>
#include <vector>

#include "Access.h"
#include "Definitions.h"

class Generator
{
public:
	Generator();
	void initialize_deques(std::vector<I32> x, std::vector<I32> y);
	I32 next_x();
	I32 next_y();
private:
	std::deque<I32> x_coordinates;
	std::deque<I32> y_coordinates;
};
