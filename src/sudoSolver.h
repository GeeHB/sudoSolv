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

#include "shared/scrCapture.h"



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

    // _initStats() : initialize grid stats
    //
    void _initStats();

    // _updateFileItemsState() : Item's state
    //
    //  @modified : Has the grid been edited (changed) ?
    //
    void _updateFileItemsState(bool modified = false);


    // _updateShortFileName() : Get short filename from FQN
    //
    void _updateShortFileName();
    
    void _displayStats(const char* fName, int8_t obvious, int elapse);

private:
    menuBar     menu_;      // Application menu
    grids       files_;     // List of grid files

    // Current file name
    uint16_t fileName_[BFILE_MAX_PATH + 1];
    char sFileName_[BFILE_MAX_PATH + 1];    // short filename in char* format
    
    // Resolution stats.
    int8_t obviousVals_;
    int duration_;

#ifndef NO_CAPTURE
    scrCapture  _capture;   // Screen capture object
#endif // #ifndef NO_CAPTURE
};

#endif // __S_SOLVER_OBJECT_h__

// EOF