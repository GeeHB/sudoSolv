//----------------------------------------------------------------------
//--
//--    main.cpp
//--
//--        Create, edit, modify and find a solution of a sudoku grid
//--
//--        App. entry point
//--
//----------------------------------------------------------------------

#ifdef DEST_CASIO_CALC
#include "sudoSolver.h"

// APP. entry-point
//
int main(void){
    sudoSolver sSolver;

    sSolver.showHomeScreen();
    sSolver.createMenu();
    sSolver.browseGridFolder();

    // Enter app. main loop
    sSolver.run();

    //gint_setrestart(0);
    gint_osmenu();

    return 1;
}
#endif // #ifdef DEST_CASIO_CALC
// EOF
