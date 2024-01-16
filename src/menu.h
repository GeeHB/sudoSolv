//----------------------------------------------------------------------
//--
//--    menu.h
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

#define IDS_EDIT            "Edit"      // "Edit" sub-menu
#define IDS_EDIT_OK         "Ok"
#define IDS_EDIT_CANCEL     "Cancel"

#define IDS_SOLVE_SUBMENU   "Solve"
#define IDS_QUIT            "Quit"

#define IDM_EDIT             200        // In the "main" menu bar
#define IDM_EDIT_OK          201
#define IDM_EDIT_CANCEL      202

#define IDM_QUIT             400

// "File" sub-menu bar
//
#define IDS_FILE_NEW        "New"
#define IDS_FILE_PREV       "Prev."
#define IDS_FILE_NEXT       "Next"
#define IDS_FILE_SAVE       "Save"
#define IDS_FILE_DELETE     "Delete"

#define IDM_FILE_SUBMENU    100
#define IDM_FILE_NEW        101
#define IDM_FILE_PREV       102
#define IDM_FILE_NEXT       103
#define IDM_FILE_SAVE       104
#define IDM_FILE_DELETE     105

// "Solve" sub-menu bar
//
#define IDS_SOLVE_OBVIOUS   "Obvious"
#define IDS_SOLVE_RESOLVE   "Resolve"
#define IDS_SOLVE_REVERT    "Revert"

#define IDM_SOLVE_SUBMENU   300
#define IDM_SOLVE_OBVIOUS   301
#define IDM_SOLVE_RESOLVE   302
#define IDM_SOLVE_REVERT    303

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

#endif // __SUDOKU_S_MENU_h__

// EOF
