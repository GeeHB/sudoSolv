#include "sudoku.h"

#define GRID_FILE      u"\\\\fls0\\GRIDS\\1.txt"

int main(void)
{
	dclear(C_WHITE);
	
	sudoku myGame;
	myGame.load((FONTCHARACTER)GRID_FILE);
	myGame.display();
	myGame.edit();
	
	myGame.findObviousValues();
	myGame.display();
	
	getkey();
	return 1;
}
