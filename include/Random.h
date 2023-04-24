#pragma once

#include <cstdio>
#include <algorithm>
#include <cstdlib>
#include <cmath>

int get_random(int a, int b)
{
	return std::min(a, b) + rand() % std::abs(b - a);
}

float get_random(float a, float b)
{
	return std::min(a, b) + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / std::abs(b - a)));
}
