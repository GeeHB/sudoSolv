#include "sudoku.h"

int main(void)
{
	dclear(C_WHITE);
	
	sudoku myGame;
	myGame.display();
	myGame.edit(KEY_CODE_EXIT);

	getkey();
	return 1;
}
