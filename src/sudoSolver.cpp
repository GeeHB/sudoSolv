//----------------------------------------------------------------------
//--
//--    sudoSolver.cpp
//--
//--        Implementation of sudoSolver object
//--
//----------------------------------------------------------------------

#include "sudoSolver.h"

extern bopti_image_t g_homeScreen;  // Background image

// Construction
//
sudoSolver::sudoSolver(){
    // Initialize members
    _initStats();
}

// Destruction
//
sudoSolver::~sudoSolver(){
#ifndef NO_CAPTURE
    if (capture_.isSet()){
       capture_.remove();  // stop "capture" on exit
    }
#endif // #ifndef NO_CAPTURE
}

// Create app. menu bar
//
void sudoSolver::createMenu(){
    // "File" sub menu
    menuBar fileMenu;
    fileMenu.appendItem(IDM_FILE_NEW, IDS_FILE_NEW);
    fileMenu.appendItem(IDM_FILE_PREV, IDS_FILE_PREV, ITEM_STATE_INACTIVE);
    fileMenu.appendItem(IDM_FILE_NEXT, IDS_FILE_NEXT, ITEM_STATE_INACTIVE);
    fileMenu.appendItem(IDM_FILE_SAVE, IDS_FILE_SAVE, ITEM_STATE_INACTIVE);
    fileMenu.appendItem(IDM_FILE_DELETE, IDS_FILE_DELETE, ITEM_STATE_INACTIVE);
    menu_.appendSubMenu(&fileMenu, IDM_FILE_SUBMENU, IDS_FILE_SUBMENU);

    menu_.appendItem(IDM_EDIT, IDS_EDIT);

    // "Solve" sub menu
    menuBar sMenu;
    sMenu.appendItem(IDM_SOLVE_OBVIOUS, IDS_SOLVE_OBVIOUS);
    sMenu.appendItem(IDM_SOLVE_RESOLVE, IDS_SOLVE_RESOLVE);
    sMenu.addItem(MENU_POS_RIGHT - 1, IDM_SOLVE_REVERT, IDS_SOLVE_REVERT);
    menu_.appendSubMenu(&sMenu, IDM_SOLVE_SUBMENU, IDS_SOLVE_SUBMENU);

    menu_.addItem(MENU_POS_RIGHT, IDM_QUIT, IDS_QUIT);

    menu_.update();
}

// showHomeScreen() : Display home screen
//
void sudoSolver::showHomeScreen(){
    drect(0, 0, CASIO_WIDTH, CASIO_HEIGHT - menu_.getHeight(), C_WHITE);
    dimage(0, 0, &g_homeScreen);

    char copyright[255];
    strcpy(copyright, APP_NAME);
    strcat(copyright, " par ");
    strcat(copyright, APP_AUTHOR);
    strcat(copyright, " v");
    strcat(copyright, APP_VERSION);

    int w, h;
    dsize(copyright, NULL, &w, &h);
    dtext(CASIO_WIDTH - w - 5,
            CASIO_HEIGHT - menu_.getHeight() - h - 10, C_BLACK,
            copyright);

    dupdate();
}

// browseGridFolder() : Browse the folder containing grid files
//
void sudoSolver::browseGridFolder(){
    // List of grid files
    if (files_.browse() > 0){
        _updateFileItemsState();
    }
}

// run() : Edit / solve sudoku(s)
//
void sudoSolver::run(void)
{
    // Start sudoku solver object
    RECT mainRect;
    menu_.getRect(mainRect);
    mainRect = {0, 0, mainRect.w, CASIO_HEIGHT - mainRect.h};
    
    game_.setScreenRect(&mainRect); // "screen" dim fir sudoku grid

    // Handle user's choices
    bool end(false);
    MENUACTION action;
    while (!end){
        // A menu action ?
        action = menu_.handleKeyboard();

        // push a menu key ?
        if (ACTION_MENU == action.type){
            switch (action.value){
                // (re)start with a new empty grid
                case IDM_FILE_NEW:
                    _onFileNew();
                    break;

                // Load previous file
                case IDM_FILE_PREV:
                    _onFilePrevious();
                    break;

                // Load next file
                case IDM_FILE_NEXT:
                    _onFileNext();
                    break;

                // Save the grid
                case IDM_FILE_SAVE:
                    _onFileSave();
                    break;
                
                // Remove current file
                case IDM_FILE_DELETE:
                    _onFileDelete();
                    break;

                // Modify current grid
                case IDM_EDIT:{
                    _onEdit();
                    break;
                }

                // Search for obvious values
                case IDM_SOLVE_OBVIOUS:{
                    _onSolveFindObvious();
                    break;
                }

                // Try to find a solution
                case IDM_SOLVE_RESOLVE:
                    _onFindSolution();
                    break;

                // Return to "original" grid
                case IDM_SOLVE_REVERT:
                    _onRevert();
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
                            _onCapture();
                        }
                        break;
#endif // #ifndef NO_CAPTURE

                    default:
                        break;
                } // switch (action.value)
            }
        }
    }
}

//
// Internal methods
//

//
// Menus
// 

// _onFileNew() : Create a new empty grid
//
void sudoSolver::_onFileNew(){
    game_.empty();
    game_.display();
    _initStats();
}

// _onFilePrevious() : Open previous file in the grid folder
//
void sudoSolver::_onFilePrevious(){
    int error(FILE_NO_ERROR);
    if (files_.prevFile(fileName_)){
        _updateFileItemsState();

        // Update grid on screen
        if (FILE_NO_ERROR == (error = game_.load(fileName_))){
            game_.display(false);
            _newFileName();                        
        }
        else{
            dprint(TEXT_X, TEXT_V_OFFSET, C_RED, 
                    "Error loading file : %d", (int)error);
            dupdate();
        }
    } 
}

// _onFileNext() : Open next file in the grid folder
//
void sudoSolver::_onFileNext(){
    int error(FILE_NO_ERROR);
    if (files_.nextFile(fileName_)){
        _updateFileItemsState();

        // Update grid on screen
        if (FILE_NO_ERROR == (error = game_.load(fileName_))){
            game_.display(false);
            _newFileName();                            
        }
        else{
            dprint(TEXT_X, TEXT_V_OFFSET, C_RED, 
                        "Error loading file : %d", (int)error);
            dupdate();
        }
    }    
}

// _onFileSave() : Save current grid
//
void sudoSolver::_onFileSave(){
    bool fileExists(sFileName_[0]);
    int uid(-1);
    int error(FILE_NO_ERROR);
    
    // Use current name or new name if none
    if (fileExists || (!fileExists && files_.getNewFileName(fileName_, &uid))){
        if (FILE_NO_ERROR == (error = game_.save(fileName_))){
            if (!fileExists){
                // It's a new file
                _newFileName();

                // Update internal list
                files_.browse();
                files_.setPos(files_.findPos(uid)); // select the file                                
            }
                    
            _updateFileItemsState();
        }   // save
        else{
            dprint(TEXT_X, TEXT_V_OFFSET, C_RED, "Error saving : %d", error);
            dupdate();
        }
    }
}

// _onFileDelete() : Delete current file (if any opened)
//
void sudoSolver::_onFileDelete(){
    if (files_.deleteFile()){
        bool file(true);
        if (!files_.prevFile(fileName_)){
            menu_.activate(IDM_FILE_PREV, SEARCH_BY_ID, false);
            if (!files_.nextFile(fileName_)){
                _updateFileItemsState();
                game_.empty();
                file = false;   // No filename
            }                            
        }

        if (!file){
            fileName_[0] = 0x0000;
        }

        _newFileName();
    }
}

// _onEdit() : Edit current grid
//
void sudoSolver::_onEdit(){
    game_.display();
    if (game_.edit()){
        _displayStats();
    }
    else{
        game_.revert();     // revert on cancel
    }

    menu_.selectByIndex(-1);
    _updateFileItemsState();
}

// _onSolveFindObvious() : Search for obvious values
//
void sudoSolver::_onSolveFindObvious(){
    obviousVals_ = game_.findObviousValues();
    duration_ = -1;
    game_.display(false);                    
    _displayStats();
}

// _onFindSolution() : Try to find a solution
//
void sudoSolver::_onFindSolution(){
    game_.display();
    if (!game_.resolve(&duration_)){
        // No soluce found ...
        // ... return to original grid
        game_.revert();
    }

    game_.display(false);
    _displayStats();
}

// _onRevert() : Return to "original" grid
//
void sudoSolver::_onRevert(){
    game_.revert();
    game_.display(false);
    obviousVals_ = -1;
    duration_ = -1;
    _displayStats();
}

// _onCapture() : Activate or deactivate capture mode
//
#ifndef NO_CAPTURE
void sudoSolver::_onCapture(){
    if (!capture_.isSet()){
        capture_.install();
    }
    else{
        capture_.remove();
    }
}
#endif // #ifndef NO_CAPTURE


// _initStats() : initialize grid stats
//  Initailizes data related to grid file and resolution
//
//  @whole : init fileName as well ?
//
void sudoSolver::_initStats(bool whole){
    if (whole){
        fileName_[0] = 0x0000;
    }

    sFileName_[0] = '\0';
    obviousVals_ = -1;
    duration_ = -1;
}

// _updateFileItemsState() : Item's state
//
//  @modified : Has the grid been edited or changed ?
//
void sudoSolver::_updateFileItemsState(bool modified){
    int count(files_.size());
    int pos(files_.pos());
    bool bPrev, bNext, bSave, bDelete;
    if (!count){
        bPrev = bNext = bSave = bDelete = false;
    }
    else{
        bPrev = (pos>0);    // Not the first in list ?
        bNext = (pos < (count - 1)); // Not the last in list
        bSave = modified;
        bDelete = (sFileName_[0]);
    }
    
    menu_.activate(IDM_FILE_PREV, SEARCH_BY_ID, bPrev);
    menu_.activate(IDM_FILE_NEXT, SEARCH_BY_ID, bNext);
    menu_.activate(IDM_FILE_SAVE, SEARCH_BY_ID, bSave);
    menu_.activate(IDM_FILE_DELETE, SEARCH_BY_ID, bDelete); 

    menu_.update();
}

// _newFileName() : Notifies FQN has changed
//
void sudoSolver::_newFileName(){
    _initStats(false);
    if (bFile::FC_len(fileName_)){
        char fName[BFILE_MAX_PATH + 1];
        bFile::FC_FC2str(fileName_, fName);
        char* name = strrchr(fName, CHAR_PATH_SEPARATOR);
        strcpy(sFileName_, (name?++name:fName)); // No path ?
    }

    _displayStats();
}

// _displayStats() : Display information about the grid and
//                   the solution if found any
//
void sudoSolver::_displayStats(){
    if (sFileName_[0]){
        dprint(TEXT_X, TEXT_Y, C_BLACK, "File : %s", sFileName_);
    }

    if (obviousVals_ != -1){
        if (obviousVals_){
            dprint(TEXT_X, TEXT_Y + TEXT_V_OFFSET, C_BLACK, 
                    "%d obvious values", obviousVals_);
        }
        else{
            dtext(TEXT_X, TEXT_Y + TEXT_V_OFFSET, C_BLACK,
                    "No obvious value");
        }
    }

    if (duration_ != -1){
        if (duration_){
            dprint(TEXT_X, TEXT_Y + 2*TEXT_V_OFFSET, C_BLACK,
                    "Solved in %d ms", duration_);
        }
        else{
            dprint(TEXT_X, TEXT_Y + 2*TEXT_V_OFFSET, C_BLACK,
                    "No solution found");
        }
    }

    dupdate();
}
    
// EOF
