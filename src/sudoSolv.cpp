//----------------------------------------------------------------------
//--
//--    sudoSolv.cpp
//--
//--        Create, edit, modify and find a solution of a sudoku grid
//--
//--        App. entry point
//--
//----------------------------------------------------------------------

#include "grids.h"
#include "sudoku.h"
#include "menu.h"

#include "shared/scrCapture.h"

extern bopti_image_t g_homeScreen;  // Background image
scrCapture  g_Capture;              // Screen capture object


// Functions definitions
//
void _createMenu(menuBar& menu);
void _homeScreen();

// APP. entry-point
//
int main(void)
{
	// Launch the application
	_homeScreen();

	// App menus
    menuBar menu;
    _createMenu(menu);
    menu.update();

    RECT mainRect;
    menu.getRect(mainRect);     // menu bar position
    mainRect = {0, 0, mainRect.w, CASIO_HEIGHT - mainRect.h};

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
                    game.display();
                    bool modified = game.edit();

                    menu.selectIndex(-1);
                    menu.update();  // redraw the whole menu bar
                    break;
                }

                // Search for obvious values
                case IDM_SOLVE_OBVIOUS:
                    game.findObviousValues();
                    game.display();
                    break;

                // Try to find a solution
                case IDM_SOLVE_FIND:
                    game.display();
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

                    // Activate or deactivate screen capture
                    case KEY_CODE_CAPTURE:
                        if (action.modifier == MOD_SHIFT){
                            if (!g_Capture.isSet()){
                                g_Capture.install();
                            }
                            else{
                                g_Capture.remove();
                            }
                        }
                        break;

                    default:
                        break;
                } // switch (action.value)
            }
        }
    }

    //gint_setrestart(1);
    gint_osmenu();

	return 1;
}

// Create menu bars
//
void _createMenu(menuBar& menu){
    // "File" sub menu
    menuBar fileMenu;
    fileMenu.appendItem(IDM_FILE_NEW, IDS_FILE_NEW);
    fileMenu.appendItem(IDM_FILE_PREV, IDS_FILE_PREV, ITEM_INACTIVE);
    fileMenu.appendItem(IDM_FILE_NEXT, IDS_FILE_NEXT, ITEM_INACTIVE);
    fileMenu.appendItem(IDM_FILE_SAVE, IDS_FILE_SAVE, ITEM_INACTIVE);
    fileMenu.appendItem(IDM_FILE_ABOUT, IDS_FILE_ABOUT);
    menu.appendSubMenu(&fileMenu, IDM_FILE_SUBMENU, IDS_FILE_SUBMENU);

    menu.appendItem(IDM_EDIT, IDS_EDIT);

    // "Solve" submenu
    menuBar sMenu;
    sMenu.appendItem(IDM_SOLVE_OBVIOUS, IDS_SOLVE_OBVIOUS);
    sMenu.appendItem(IDM_SOLVE_FIND, IDS_SOLVE_FIND);
    menu.appendSubMenu(&sMenu, IDM_SOLVE_SUBMENU, IDS_SOLVE_SUBMENU);

    menu.addItem(ITEM_POS_RIGHT, IDM_QUIT, IDS_QUIT);
}

// Display home screen
//
void _homeScreen(){
    drect(0,0,CASIO_WIDTH, CASIO_HEIGHT - MENUBAR_DEF_HEIGHT, C_WHITE);
    dimage(0,0,&g_homeScreen);

    char copyright[255];
    strcpy(copyright, APP_NAME);
    strcat(copyright, " par ");
    strcat(copyright, APP_AUTHOR);
    strcat(copyright, " v");
    strcat(copyright, APP_VERSION);

    int w, h;
    dsize(copyright, NULL, &w, &h);
    dtext(CASIO_WIDTH - w - 5,
		CASIO_HEIGHT - MENUBAR_DEF_HEIGHT - h - 10,
		COLOUR_BLACK,
		copyright);

    dupdate();
}

// EOF
