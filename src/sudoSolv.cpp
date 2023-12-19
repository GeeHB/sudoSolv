//---------------------------------------------------------------------------
//--
//--    sudoSolv.cpp
//--
//--        Create, edit, modify and find a solution of a sudoku grid
//--
//--        App. entry point
//--
//---------------------------------------------------------------------------

#include "sudoku.h"
#include "menu.h"

#define GRID_FILE      u"\\\\fls0\\GRIDS\\1.txt"

// Background image
#ifdef DEST_CASIO_CALC
extern bopti_image_t g_homeScreen;
#endif // #ifdef DEST_CASIO_CALC

// Functions definitions
//
void _createMenu(menuBar& menu);
void _homeScreen();

// APP. entry-point
//
int main(void)
{
	// Launch the application
	dclear(C_WHITE);
	_homeScreen();

	// App menus
    menuBar menu;
    _createMenu(menu);
    menu.update();

    RECT mainRect;
    menu.getRect(mainRect);     // menu bar position
    mainRect = {0, 0, mainRect.w, CASIO_HEIGHT - mainRect.h};   // Screen dimensions

    //
    // App. main loop
    //
    sudoku game(&mainRect);
    bool end(false);
    MENUACTION action;
    while (!end){
        // A menu action ?
        action = menu.handleKeyboard();

        // ... from menu ?
        if (ACTION_MENU == action.type){
            switch (action.value){

                // (re)start with a new empty grid
                case IDM_FILE_NEW:
                    game.empty();
                    game.display();
                    break;

                case IDM_FILE_ABOUT:
                    _homeScreen();
                    break;

                // Modify current grid
                case IDM_EDIT:{
                    bool modified = game.edit();
                    break;
                }

                // Search for obvious values
                case IDM_SOLVE_OBVIOUS:
                    game.findObviousValues();
                    game.display();
                    break;

                // Try to find a solution
                case IDM_SOLVE_FIND:
                    if (!game.resolve()){
                        // No soluce found ...
                        // ... return to original grid
                        game.revert();
                    }
                    game.display();
                    break;

                case IDM_QUIT:
                    end = true;
                    break;

                default:
                    break;  // ???
            } // switch
        }else{
            // From keyboard (and not handled by the menu)
            if (ACTION_KEYBOARD == action.type){
                switch (action.value){
                    // Return to main menu
                    case KEY_MENU:
                        end = true;
                        break;

                    default:
                        break;
                } // switch (action.value)
            }
        }
    }

#ifdef DEST_CASIO_CALC
    //gint_setrestart(1);
    gint_osmenu();
#endif // #ifdef DEST_CASIO_CALC

	return 1;
}

// Create menu bars
//
void _createMenu(menuBar& menu){
    // "File" sub menu
    //menuBar* subMenu = menu.createSubMenu(); // equivalent to new menuBar()
    menuBar fileMenu;
    fileMenu.appendItem(IDM_FILE_NEW, IDS_FILE_NEW);
    fileMenu.appendItem(IDM_FILE_PREV, IDS_FILE_PREV, ITEM_INACTIVE);
    fileMenu.appendItem(IDM_FILE_NEXT, IDS_FILE_NEXT, ITEM_INACTIVE);
    fileMenu.appendItem(IDM_FILE_SAVE, IDS_FILE_SAVE, ITEM_INACTIVE);
    fileMenu.appendItem(IDM_FILE_ABOUT, IDS_FILE_ABOUT);
    menu.appendSubMenu(&fileMenu, IDM_FILE_SUBMENU, IDS_FILE_SUBMENU);

    menu.appendItem(IDM_EDIT, IDS_EDIT);

    // "Solve" submenu
    menuBar solveMenu;
    solveMenu.appendItem(IDM_SOLVE_OBVIOUS, IDS_SOLVE_OBVIOUS);
    solveMenu.appendItem(IDM_SOLVE_FIND, IDS_SOLVE_FIND);
    menu.appendSubMenu(&solveMenu, IDM_SOLVE_SUBMENU, IDS_SOLVE_SUBMENU);

    menu.addItem(ITEM_POS_RIGHT, IDM_QUIT, IDS_QUIT);
}

// Display home screen
//
void _homeScreen(){
#ifdef DEST_CASIO_CALC
    dclear(C_WHITE);
    dimage(0, 0, &g_homeScreen);

    char copyright[255];
    strcpy(copyright, APP_NAME);
    strcat(copyright, " par ");
    strcat(copyright, APP_AUTHOR);
    strcat(copyright, " v");
    strcat(copyright, APP_VERSION);

    int w, h;
    dsize(copyright, NULL, &w, &h);
    dtext(CASIO_WIDTH - w - 5, CASIO_HEIGHT - MENUBAR_DEF_HEIGHT - h - 10, COLOUR_BLACK, copyright);

    dupdate();
#endif // #ifdef DEST_CASIO_CALC
}

// EOF
