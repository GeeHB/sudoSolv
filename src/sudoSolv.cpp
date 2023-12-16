//---------------------------------------------------------------------------
//--
//--    sudoSolv.cpp
//--
//--        Create, edit, modify and find a solution to a sudoku grid
//--
//--        App. entry point
//--
//---------------------------------------------------------------------------

#include "sudoku.h"
#include "menu.h"

#define GRID_FILE      u"\\\\fls0\\GRIDS\\1.txt"

int main(void)
{
	dclear(C_WHITE);

    sudoku game;
	menuBar menu;

	//
	// Create menus
	//

    // "File" sub menu
    //menuBar* subMenu = menu.createSubMenu(); // equivalent to new menuBar()
    //menuBar* subMenu = new menuBar();
    {
        menuBar subMenu;
        subMenu.addItem(IDM_FILE_NEW, IDS_FILE_NEW);
        subMenu.addItem(IDM_FILE_PREV, IDS_FILE_PREV, ITEM_INACTIVE);
        subMenu.addItem(IDM_FILE_NEXT, IDS_FILE_NEXT, ITEM_INACTIVE);
        subMenu.addItem(IDM_FILE_SAVE, IDS_FILE_SAVE, ITEM_INACTIVE);
        menu.addSubMenu(&subMenu, IDS_FILE_SUB);
        //delete subMenu; // Don't need sub menu any longer !
    }

    menu.addItem(IDM_EDIT, IDS_EDIT);

    // "Solver" submenu
    {
        menuBar subMenu;
        subMenu.addItem(IDM_SOLVER_OBVIOUS, IDS_SOLVER_OBVIOUS);
        subMenu.addItem(IDM_SOLVER_FIND, IDS_SOLVER_FIND);
        menu.addSubMenu(&subMenu, IDS_SOLVER_SUB);
        //delete subMenu;
    }

    menu.addItem(IDM_QUIT, IDS_QUIT);
    menu.update();

    //
    // App. main loop
    //
    bool end(false);
    MENUACTION action;
    while (!end){
        // A menu action ?
        action = menu.handleKeyboard();

        // from menu ?
        if (ACTION_MENU == action.type){
            switch (action.value){

                case IDM_FILE_NEW:
                    game.empty();
                    game.display();
                    break;

                case IDM_EDIT:{
                    bool modified = game.edit();
                    break;
                }

                case IDM_SOLVER_OBVIOUS:
                    game.findObviousValues();
                    game.display();
                    break;

                case IDM_SOLVER_FIND:
                    game.resolve();
                    game.display();
                    break;

                case IDM_QUIT:
                    end = true;
                    break;

                default:
                    break;  // ???
            }
        }
    }

#ifdef DEST_CASIO_CALC
    //gint_setrestart(1);
    gint_osmenu();
#endif // #ifdef DEST_CASIO_CALC

	return 1;
}
