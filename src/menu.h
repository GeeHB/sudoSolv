
//---------------------------------------------------------------------------
//--
//--    menu.h
//--
//--        Definition menu constants
//--
//---------------------------------------------------------------------------

#ifndef __SUDOKU_S_MENU_h__
#define __SUDOKU_S_MENU_h__    1

#include "shared/menuBar.h"

// Main menu
//
#define IDS_FILE_SUB        "File"
#define IDS_EDIT            "Edit"
#define IDS_SOLVER_SUB      "Solver"
#define IDS_QUIT            "Quit"

#define IDM_EDIT             200
#define IDM_QUIT             400


// "File" menu bar
//
#define IDS_FILE_NEW        "New"
#define IDS_FILE_PREV       "Previous"
#define IDS_FILE_NEXT       "Next"
#define IDS_FILE_SAVE       "Save"

#define IDM_FILE_NEW        101
#define IDM_FILE_PREV       102
#define IDM_FILE_NEXT       103
#define IDM_FILE_SAVE       104

// "Solver" menu bar
//
#define IDS_SOLVER_OBVIOUS  "Obvious"
#define IDS_SOLVER_FIND     "Find"

#define IDM_SOLVER_OBVIOUS  301
#define IDM_SOLVER_FIND     302

#endif // __SUDOKU_S_MENU_h__

// EOF
