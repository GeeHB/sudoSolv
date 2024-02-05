//----------------------------------------------------------------------
//--
//--    menus.h
//--
//--        Definition of constants for menu bars
//--
//----------------------------------------------------------------------

#ifndef __SUDOKU_S_MENU_h__
#define __SUDOKU_S_MENU_h__    1

#include "shared/menuBar.h"

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

// Main menu
//

#define IDS_FILE_SUBMENU    "File"
#define IDM_FILE_SUBMENU    1
#define IDS_EDIT            "Edit"
#define IDM_EDIT            2
#define IDS_SOLVE_SUBMENU   "Solve"
#define IDM_SOLVE_SUBMENU   3
#define IDS_QUIT            "Quit"
#define IDM_QUIT            4


// "File" sub-menu bar
//
#define IDS_FILE_NEW        "New"
#define IDM_FILE_NEW        11
#define IDS_FILE_PREV       "Prev."
#define IDM_FILE_PREV       12
#define IDS_FILE_NEXT       "Next"
#define IDM_FILE_NEXT       13
#define IDS_FILE_SAVE       "Save"
#define IDM_FILE_SAVE       14
#define IDS_FILE_DELETE     "Delete"
#define IDM_FILE_DELETE     15


// Edit - Creation mode
#define IDS_EDIT_OK         "Ok"
#define IDM_EDIT_OK          21
#define IDS_EDIT_CANCEL     "Cancel"
#define IDM_EDIT_CANCEL      22


// "Solve" sub-menu bar
//
#define IDS_SOLVE_MANUAL_SUBMENU    "Manual"
#define IDM_SOLVE_MANUAL_SUBMENU    31
#define IDS_SOLVE_OBVIOUS           "Obvious"
#define IDM_SOLVE_OBVIOUS           32
#define IDS_SOLVE_RESOLVE           "Resolve"
#define IDM_SOLVE_RESOLVE           33
#define IDS_SOLVE_REVERT            "Revert"
#define IDM_SOLVE_REVERT            34


// Solve/Edit - Manual
//
#define IDS_MANUAL_SUBMENU  "Col."
#define IDM_MANUAL_SUBMENU  311
#define IDS_MANUAL_MERGE    "Merge"
#define IDM_MANUAL_MERGE    312
#define IDS_MANUAL_REMOVE   "Remove"
#define IDM_MANUAL_REMOVE   313
#define IDS_MANUAL_END      "End"
#define IDM_MANUAL_END      IDM_EDIT_OK

#define IDM_MANUAL_COLOUR_FIRST   3111      // MANUAL_HYP submenu

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

#endif // __SUDOKU_S_MENU_h__

// EOF