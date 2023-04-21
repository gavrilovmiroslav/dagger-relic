#include "LevelManager.h"

void LevelManager::load_level(String file)
{
	int row_count = 0;
	int col_count = 0;

	level_file.open(file);
	if (level_file.is_open())
	{
		Char in_char;
		while (level_file.get(in_char))
		{
			//TODO: Windows problems
			if(in_char == '\r')
			{
				continue;
			}
			if (in_char == '\n')
			{
				col_count = 0;
				row_count++;
			}
			else
			{
				level_map[row_count][col_count] = in_char;
				col_count++;
			}
		}
	}

	level_file.close();
}