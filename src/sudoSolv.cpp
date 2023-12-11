#include "sudoku.h"

#define GRID_FILE      u"\\\\fls0\\GRIDS\\1.txt"

int main(void)
{
	dclear(C_WHITE);
	
	sudoku myGame;
	
	if (myGame.load((FONTCHARACTER)GRID_FILE)){
		myGame.display();
		
		myGame.edit();
		
		// Any obvious value(s) ?
		if (myGame.findObviousValues()){
			myGame.display();
			getkey();
		}
		
		// A solution ?
		if (myGame.resolve()){
			// yes !!!
			myGame.display();
			getkey();
		}
	}

	return 1;
}
