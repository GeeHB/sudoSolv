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

#ifndef NO_CAPTURE
scrCapture  g_Capture;              // Screen capture object
#endif // #ifndef NO_CAPTURE

// Functions definitions
//
void _createMenu(menuBar& menu);
void _homeScreen();
void _setFileName(FONTCHARACTER fullName, char* shortName);
void _displayStats(const char* fName, int8_t obvious, int elapse);

// APP. entry-point
//
int main(void)
{
    _homeScreen();

    // Create app. menus
    menuBar menu;
    _createMenu(menu);
    menu.update();

    RECT mainRect;
    menu.getRect(mainRect);     // menu bar position
    mainRect = {0, 0, mainRect.w, CASIO_HEIGHT - mainRect.h};

    // List of grid files
    grids files;
    if (files.size()){
        menu.activate(IDM_FILE_NEXT, SEARCH_BY_ID, true);
        menu.update();
    }

    // Stats.
    uint16_t fileName[BFILE_MAX_PATH + 1];
    char sFileName[BFILE_MAX_PATH + 1];
    int8_t obviousVals(-1);
    int duration(-1);

    // App. main loop
    //
    sudoku game(&mainRect);
    bool end(false);
    uint8_t error(FILE_NO_ERROR);
    MENUACTION action;
    while (!end){
        // A menu action ?
        action = menu.handleKeyboard();

        // push a menu key ?
        if (ACTION_MENU == action.type){
            switch (action.value){
                // (re)start with a new empty grid
                case IDM_FILE_NEW:
                    game.empty();
                    game.display();
                    sFileName[0] = '\0';
                    obviousVals = duration = -1;
                    break;

                // Load previous file
                case IDM_FILE_PREV:
                    if (files.prevFile(fileName)){
                        menu.activate(IDM_FILE_NEXT, SEARCH_BY_ID, true);
                        menu.activate(IDM_FILE_DELETE, SEARCH_BY_ID, true);
                        if (!files.pos()){
                            menu.activate(IDM_FILE_PREV, SEARCH_BY_ID, false);
                        }
                        menu.update();

                        // Update grid on screen
                        if (FILE_NO_ERROR == (error = game.load(fileName))){
                            game.display(false);
                            _setFileName(fileName, sFileName);
                            _displayStats(sFileName, -1, -1);
                        }
                        else{
                            dprint(TEXT_X, TEXT_V_OFFSET, C_RED, "Error loading file : %d", (int)error);
                            dupdate();
                        }
                    } 
                    break;

                // Load next file
                case IDM_FILE_NEXT:
                    if (files.nextFile(fileName)){
                        // Menu changes
                        menu.activate(IDM_FILE_PREV, SEARCH_BY_ID, true);
                        menu.activate(IDM_FILE_DELETE, SEARCH_BY_ID, true);
                        if (files.pos() >= (files.size() - 1)){
                            menu.activate(IDM_FILE_NEXT, SEARCH_BY_ID, false);
                        }
                        menu.update();

                        // Update grid on screen
                        if (FILE_NO_ERROR == (error = game.load(fileName))){
                            game.display(false);
                            _setFileName(fileName, sFileName);
                            _displayStats(sFileName, -1, -1);
                        }
                        else{
                            dprint(TEXT_X, TEXT_V_OFFSET, C_RED, "Error loading file : %d", (int)error);
                            dupdate();
                        }
                    }
                    else{
                        menu.activate(IDM_FILE_NEXT, SEARCH_BY_ID, false);
                        menu.update();
                    }
                    break;

                // Save the grid (using a new name)
                case IDM_FILE_SAVE:
                    if (files.newFileName(fileName)){
                        if (game.save(fileName)){
                            if (files.addFileName(fileName)){
                                _setFileName(fileName, sFileName);
                                _displayStats(sFileName, -1, -1);

                                // New filename was added at list's tail
                                files.setPos(files.size() - 1);
                                menu.activate(IDM_FILE_NEXT, SEARCH_BY_ID, false);
                                menu.activate(IDM_FILE_PREV, SEARCH_BY_ID, files.size()>1);
                            }
                        }
                        else{
                            dprint(TEXT_X, TEXT_V_OFFSET, C_RED, "Error saving file");
                        }
                        menu.activate(IDM_FILE_SAVE, SEARCH_BY_ID, false);
                        menu.activate(IDM_FILE_DELETE, SEARCH_BY_ID, true);
                        menu.update();
                    }
                    break;
                
                // Remove current file
                case IDM_FILE_DELETE:
                    if (files.deleteFile()){
                        bool file(true);
                        if (!files.prevFile(fileName)){
                            menu.activate(IDM_FILE_PREV, SEARCH_BY_ID, false);
                            if (!files.nextFile(fileName)){
                                menu.activate(IDM_FILE_NEXT, SEARCH_BY_ID, false);
                                menu.activate(IDM_FILE_DELETE, SEARCH_BY_ID, false);
                                game.empty();
                                file = false;   // No filename
                            }    
                            menu.update();
                        }

                        _setFileName(file?fileName:NULL, sFileName);
                        _displayStats(sFileName, -1, -1);
                    }
                    break;

                // Modify current grid
                case IDM_EDIT:{
                    game.display();
                    if (game.edit()){
                        menu.activate(IDM_FILE_SAVE, SEARCH_BY_ID, true);
                        _displayStats(sFileName, -1, -1);
                    }

                    menu.selectByIndex(-1);
                    menu.update();  // redraw the whole menu bar
                    break;
                }

                // Search for obvious values
                case IDM_SOLVE_OBVIOUS:{
                    obviousVals = game.findObviousValues();
                    game.display(false);
                    
                    _displayStats(sFileName, obviousVals, -1);
                    break;
                }

                // Try to find a solution
                case IDM_SOLVE_FIND:
                    game.display();
                    if (!game.resolve(&duration)){
                        // No soluce found ...
                        // ... return to original grid
                        game.revert();
                    }

                    game.display(false);
                    _displayStats(sFileName, obviousVals, duration);
                    
                    break;

                // Return to "original" grid
                case IDM_SOLVE_REVERT:
                    game.revert();
                    game.display(false);
                    obviousVals = -1;
                    _displayStats(sFileName, -1, -1);
                    break;

                // Quit the application
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
#ifndef NO_CAPTURE
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
#endif // #ifndef NO_CAPTURE

                    default:
                        break;
                } // switch (action.value)
            }
        }
    }

    gint_setrestart(1);
    //gint_osmenu();

	return 1;
}

// Create menu bar
//
void _createMenu(menuBar& menu){
    // "File" sub menu
    menuBar fileMenu;
    fileMenu.appendItem(IDM_FILE_NEW, IDS_FILE_NEW);
    fileMenu.appendItem(IDM_FILE_PREV, IDS_FILE_PREV, ITEM_STATE_INACTIVE);
    fileMenu.appendItem(IDM_FILE_NEXT, IDS_FILE_NEXT, ITEM_STATE_INACTIVE);
    fileMenu.appendItem(IDM_FILE_SAVE, IDS_FILE_SAVE, ITEM_STATE_INACTIVE);
    fileMenu.appendItem(IDM_FILE_DELETE, IDS_FILE_DELETE, ITEM_STATE_INACTIVE);
    menu.appendSubMenu(&fileMenu, IDM_FILE_SUBMENU, IDS_FILE_SUBMENU);

    menu.appendItem(IDM_EDIT, IDS_EDIT);

    // "Solve" submenu
    menuBar sMenu;
    sMenu.appendItem(IDM_SOLVE_OBVIOUS, IDS_SOLVE_OBVIOUS);
    sMenu.appendItem(IDM_SOLVE_FIND, IDS_SOLVE_FIND);
    sMenu.addItem(MENU_POS_RIGHT - 1, IDM_SOLVE_REVERT, IDS_SOLVE_REVERT);
    menu.appendSubMenu(&sMenu, IDM_SOLVE_SUBMENU, IDS_SOLVE_SUBMENU);

    menu.addItem(MENU_POS_RIGHT, IDM_QUIT, IDS_QUIT);
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
		CASIO_HEIGHT - MENUBAR_DEF_HEIGHT - h - 10, COLOUR_BLACK,
		copyright);

    dupdate();
}

// A new filename => get short filename
void _setFileName(FONTCHARACTER fullName, char* shortName){
    if (bFile::FC_len(fullName)){
        char fName[BFILE_MAX_PATH + 1];
        bFile::FC_FC2str(fullName, fName);
        char* name = strrchr(fName, CHAR_PATH_SEPARATOR);
        strcpy(shortName, (name?++name:fName)); // No path ?
    }
    else{
        shortName[0] = '\0';    // No filename
    }
}

// Display grid's stats
void _displayStats(const char* fName, int8_t obvious, int elapse){
    if (fName && fName[0]){
        dprint(TEXT_X, TEXT_Y, C_BLACK, "File : %s", fName);
    }

    if (obvious != -1){
        if (obvious){
            dprint(TEXT_X, TEXT_Y + TEXT_V_OFFSET, C_BLACK, "%d obvious values", obvious);

        }
        else{
            dtext(TEXT_X, TEXT_Y + TEXT_V_OFFSET, C_BLACK, "No obvious value");
        }
    }

    if (elapse != -1){
        if (elapse){
            dprint(TEXT_X, TEXT_Y + 2*TEXT_V_OFFSET, C_BLACK, "Solved in %d ms", elapse);

        }
        else{
            dprint(TEXT_X, TEXT_Y + 2*TEXT_V_OFFSET, C_BLACK, "No solution found");
        }
    }

    dupdate();
}
    
// EOF
