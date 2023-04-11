#include "LevelManager.h"

void LevelManager::LoadLevel(String file) {
	int rowCount = 0;
	int colCount = 0;

	levelFile.open(file);
	if (levelFile.is_open()) {
		Char inChar;
		while (levelFile.get(inChar)) {
			//TODO: Windows problems
			if( inChar == '\r')	
				continue; 
			if (inChar == '\n') {
				colCount = 0;
				rowCount++;
			}
			else {
				levelMap[colCount][rowCount] = inChar;
				colCount++;
			}
		}
	}

	levelFile.close();

    for(int i = 0; i < rowCount; i++){
        for(int j = 0; j < colCount; j++){
            std::cout << levelMap[j][i];
        }
        std::cout << std::endl;
    }
}