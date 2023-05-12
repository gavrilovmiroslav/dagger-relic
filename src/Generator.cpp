#include "Generator.h"

Generator::Generator() {}

void Generator::initialize_deques(std::vector<I32> x, std::vector<I32> y, std::vector<String> spell_vector)
{
	for(I32 element : x)
	{
		x_coordinates.push_back(element);
	}
	for(I32 element : y)
	{
		y_coordinates.push_back(element);
	}
	for(String element : spell_vector)
	{
		spells.push_back(element);
	}
	std::random_shuffle(x_coordinates.begin(), x_coordinates.end());
	x_coordinates.push_back(-1);
	std::random_shuffle(y_coordinates.begin(), y_coordinates.end());
	y_coordinates.push_back(-1);
	std::random_shuffle(spells.begin(), spells.end());
	spells.push_back("-1");
}

I32 Generator::next_x()
{
	I32 next_element = x_coordinates.front();
	x_coordinates.pop_front();
	if (next_element == -1)
	{
		std::random_shuffle(x_coordinates.begin(), x_coordinates.end());
		x_coordinates.push_back(-1);
		return this->next_x();
	}
	else
	{
		x_coordinates.push_back(next_element);
		return next_element;
	}
}

I32 Generator::next_y()
{
	I32 next_element = y_coordinates.front();
	y_coordinates.pop_front();
	if (next_element == -1)
	{
		std::random_shuffle(y_coordinates.begin(), y_coordinates.end());
		y_coordinates.push_back(-1);
		return this->next_y();
	}
	else
	{
		y_coordinates.push_back(next_element);
		return next_element;
	}
}

String Generator::next_spell()
{
	String next_element = spells.front();
	spells.pop_front();
	if (next_element == "-1")
	{
		std::random_shuffle(spells.begin(), spells.end());
		spells.push_back("-1");
		return this->next_spell();
	}
	else
	{
		spells.push_back(next_element);
		return next_element;
	}
}
