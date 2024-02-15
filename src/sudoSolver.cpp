//----------------------------------------------------------------------
//--
//--    sudoSolver.cpp
//--
//--        Implementation of sudoSolver object
//--
//----------------------------------------------------------------------

#ifdef DEST_CASIO_CALC

#include "sudoSolver.h"

extern bopti_image_t g_homeScreen;  // Background image

// Construction
//
sudoSolver::sudoSolver(){
    // Initialize members
    _initStats();
}

// Create app. menu bar
//
void sudoSolver::createMenu(){

    // "File/New" sub-menu
    menuBar newMenu;
    newMenu.appendItem(IDM_NEW_EMPTY, IDS_NEW_EMPTY);
    newMenu.appendItem(IDM_NEW_EASY, IDS_NEW_EASY);
    newMenu.appendItem(IDM_NEW_MEDIUM, IDS_NEW_MEDIUM);
    newMenu.appendItem(IDM_NEW_HARD, IDS_NEW_HARD);

    // "File" sub-menu
    menuBar fileMenu;
    fileMenu.appendSubMenu(&newMenu,
                        IDM_FILE_NEW_SUBMENU, IDS_FILE_NEW_SUBMENU);
    fileMenu.appendItem(IDM_FILE_PREV, IDS_FILE_PREV, ITEM_STATE_INACTIVE);
    fileMenu.appendItem(IDM_FILE_NEXT, IDS_FILE_NEXT, ITEM_STATE_INACTIVE);
    fileMenu.appendItem(IDM_FILE_SAVE, IDS_FILE_SAVE, ITEM_STATE_INACTIVE);
    fileMenu.appendItem(IDM_FILE_DELETE, IDS_FILE_DELETE, ITEM_STATE_INACTIVE);
    menu_.appendSubMenu(&fileMenu, IDM_FILE_SUBMENU, IDS_FILE_SUBMENU);

    menu_.appendItem(IDM_EDIT, IDS_EDIT);

    // "Solve" sub-menu
    menuBar sMenu;
    sMenu.appendItem(IDM_SOLVE_MANUAL_SUBMENU, IDS_SOLVE_MANUAL_SUBMENU);
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
    strcat(copyright, " by ");
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

    capture_.pause();
    int count = files_.browse();    // Update folder content
    capture_.resume();

    if (count >= 0){
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

    game_.setScreenRect(&mainRect); // "screen" dim for sudoku grid

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
                case IDM_NEW_EMPTY:
                    _onNewEmpty();
                    break;

                // Create new sudokus
                case IDM_NEW_EASY:
                    game_.create(COMPLEXITY_EASY);
                    game_.display();
                    break;
                case IDM_NEW_MEDIUM:
                    game_.create(COMPLEXITY_MEDIUM);
                    game_.display();
                    break;
                case IDM_NEW_HARD:
                    game_.create(COMPLEXITY_HARD);
                    game_.display();
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

                // Try to solve "manually" the grid
                case IDM_SOLVE_MANUAL_SUBMENU:{
                    _onSolveManual();
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
                    case KEY_CODE_CAPTURE:
                        //if (MOD_SHIFT == (action.modifier & MOD_SHIFT)){
                            _onCapture();
                        //}
                        break;

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
// Menu
//

// _onNewEmpty() : Create a new empty grid
//
void sudoSolver::_onNewEmpty(){
    game_.empty();
    game_.display();
    _initStats();

    _updateFileItemsState();
}

// _onFilePrevious() : Open previous file in the grid folder
//
void sudoSolver::_onFilePrevious(){
    uint16_t fName[BFILE_MAX_PATH + 1];
    FC_EMPTY(fName);
    if (files_.prevFile(fName)){
        _loadFile(fName);
    }
}

// _onFileNext() : Open next file in the grid folder
//
void sudoSolver::_onFileNext(){
    uint16_t fName[BFILE_MAX_PATH + 1];
    FC_EMPTY(fName);
    if (files_.nextFile(fName)){
        _loadFile(fName);
    }
}

// _onFileSave() : Save current grid
//
void sudoSolver::_onFileSave(){
    bool fileExists(sFileName_[0]);
    int uid(-1);
    int error(FILE_NO_ERROR);

    // Use current name or new name if none
    uint16_t fName[BFILE_MAX_PATH + 1];
    FC_EMPTY(fName);
    if (fileExists || (!fileExists && files_.getNewFileName(fName, &uid))){
        if (FILE_NO_ERROR == (error = game_.save(fName))){
            if (!fileExists){
                // It's a new file
                _newFileName(fName);

                // Update internal list
                capture_.pause();
                files_.browse();
                capture_.resume();

                files_.setPos(files_.findByID(uid)); // select the file
            }

            _updateFileItemsState();
        }   // save
        else{
            dprint(TEXT_X, TEXT_V_OFFSET, C_RED,
                    "Error saving : %d", error);
            dupdate();
        }
    }
}

// _onFileDelete() : Delete current file (if any opened)
//
void sudoSolver::_onFileDelete(){
    capture_.pause();

    // Try to remove the file
    bool done(false);
    if (files_.deleteFile()){
        // Update the list
        if (files_.browse()){
            files_.setPos(0);
            uint16_t fileName[BFILE_MAX_PATH + 1];
            if (files_.currentFileName(fileName)){
                done = _loadFile(fileName);
            }
        }
    }

    capture_.resume();

    // An error ?
    if (!done){
        FC_EMPTY(fileName_);    // There is no more open file
        _onNewEmpty();
    }
}

// _onEdit() : Edit current grid
//
void sudoSolver::_onEdit(){
    bool modified;
    game_.display();
    if ((modified = game_.edit(EDIT_MODE_CREATION))){
        _displayStats();
    }

    menu_.selectByIndex(-1);
    _updateFileItemsState(modified);
}

// _onSolveManual() : Try to "manually" solve the grid
//
void sudoSolver::_onSolveManual(){
    bool modified;
    game_.display();
    if ((modified = game_.edit(EDIT_MODE_MANUAL))){
        _displayStats();
    }

    menu_.selectByIndex(-1);
    _updateFileItemsState(modified);
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
void sudoSolver::_onCapture(){
    if (!capture_.isSet()){
        capture_.install();
    }
    else{
        capture_.remove();
    }
}

// _loadFile() : Load and display a grid
//
//  @fName : File to load
//
//  @return : true if successfullu loaded
//
bool sudoSolver::_loadFile(FONTCHARACTER fName){
    if (FC_ISEMPTY(fName)){
        return false;
    }

    int error(FILE_NO_ERROR);

    // Update grid on screen
    //
    capture_.pause();   // pause if installed
    error = game_.load(fName);
    capture_.resume();
    if (FILE_NO_ERROR == error){
        game_.display(false);
        _newFileName(fName);
        _updateFileItemsState();
        return true;
    }

    dprint(TEXT_X, TEXT_V_OFFSET, C_RED, "Error loading file : %d",
        (int)error);
    dupdate();
    return false;
}


// _initStats() : initialize grid stats
//  Initailizes data related to grid file and resolution
//
//  @whole : init fileName as well ?
//
void sudoSolver::_initStats(bool whole){
    if (whole){
        FC_EMPTY(fileName_);
    }

    sFileName_[0] = '\0';
    obviousVals_ = -1;
    duration_ = -1;
}

// _updateFileItemsState() : Item's state
//
//  @modified : Has the grid been edited or changed ?
//
void sudoSolver::_updateFileItemsState( bool modified){
    int count(files_.size());
    int pos(files_.pos());
    bool bPrev(false), bNext(false), bDelete(false);
    if (count){
        bPrev = (pos>0);    // Not the first in list ?
        bNext = (pos < (count - 1)); // Not the last in list
        bDelete = !FC_ISEMPTY(fileName_);
    }

    menu_.activate(IDM_FILE_PREV, SEARCH_BY_ID, bPrev);
    menu_.activate(IDM_FILE_NEXT, SEARCH_BY_ID, bNext);
    menu_.activate(IDM_FILE_SAVE, SEARCH_BY_ID, modified);
    menu_.activate(IDM_FILE_DELETE, SEARCH_BY_ID, bDelete);

    menu_.update();
}

// _newFileName() : Notifies FQN has changed
//
//  @fName : New FQN of grid
//
void sudoSolver::_newFileName(FONTCHARACTER fName){
    _initStats(false);
    if (!FC_ISEMPTY(fName)){
        bFile::FC_cpy(fileName_, fName);
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

#endif // #ifdef DEST_CASIO_CALC

// EOF
