#include "sudoku.h"

int main(void)
{
	dclear(C_WHITE);
	
	sudoku myGame;
	myGame.display();
	myGame.edit();
	
	myGame.findObviousValues();
	myGame.display();
	
	getkey();
	return 1;
}
