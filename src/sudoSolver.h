//----------------------------------------------------------------------
//--
//--    sudoSolver.h
//--
//--        Definition of sudoSolver object 
//--
//----------------------------------------------------------------------

#ifndef __S_SOLVER_OBJECT_h__
#define __S_SOLVER_OBJECT_h__    1

#include "menu.h"
#include "grids.h"
#include "sudoku.h"

#include "shared/scrCapture.h"

// sudoSolver - Application
//
class sudoSolver{
public:
    // Construction
    sudoSolver();

    // Destruction
    ~sudoSolver();

    // Create app. menu bar
    //
    void createMenu();

    // showHomeScreen() : Display home screen
    //
    void showHomeScreen();

    // browseGridFolder() : Browse the folder containing grid files
    //
    void browseGridFolder();

    // run() : Edit / solve sudoku(s)
    //
    void run();

private:

    // _onFileNew() : Create a new empty grid
    //
    void _onFileNew();
    
    // _onFilePrevious() : Open previous file in the grid folder
    //
    void _onFilePrevious();
    
    // _onFileNext() : Open next file in the grid folder
    //
    void _onFileNext();
    
    // _onFileSave() : Save current grid
    //
    void _onFileSave();
    
    // _onFileDelete() : Delete current file (if any opened)
    //
    void _onFileDelete();
    
    // _onEdit() : Edit current grid
    //
    void _onEdit();

    // _onSolveFindObvious() : Search for obvious values
    //
    void _onSolveFindObvious();

    // _onFindSolution() : Try to find a solution
    //
    void _onFindSolution();

    // _onRevert() : Return to "original" grid
    //
    void _onRevert();

    // _onCapture() : Activate or deactivate capture mode
    //
#ifndef NO_CAPTURE
    void _onCapture();
#endif // #ifndef NO_CAPTURE
    
    // _initStats() : initialize grid stats
    //  Initailizes data related to grid file and resolution
    //
    //  @whole : init fileName as well ?
    //
    void _initStats(bool whole = true);

    // _updateFileItemsState() : Item's state
    //
    //  @modified : Has the grid been edited or changed ?
    //
    void _updateFileItemsState(bool modified = false);


    // _newFileName() : Notifies FQN has changed
    //
    void _newFileName();
    
    // _displayStats() : Display information about the grid and
    //                   the solution if found any
    //
    void _displayStats();

private:
    menuBar     menu_;      // Application menu
    grids       files_;     // List of grid files
    
    sudoku      game_;      // the solver ...

    // Current file name
    uint16_t fileName_[BFILE_MAX_PATH + 1];
    char sFileName_[BFILE_MAX_PATH + 1];    // short filename as char*
    
    // Resolution stats.
    int8_t obviousVals_;
    int duration_;

#ifndef NO_CAPTURE
    scrCapture capture_;   // Screen capture object
#endif // #ifndef NO_CAPTURE
};

#endif // __S_SOLVER_OBJECT_h__

// EOF
