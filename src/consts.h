//----------------------------------------------------------------------
//--
//--    consts.h
//--
//--        App. constants
//--
//----------------------------------------------------------------------

#ifndef __S_SOLVER_CONSTANTS_h__
#define __S_SOLVER_CONSTANTS_h__    1

#include "shared/casioCalcs.h"

#ifdef DEST_CASIO_CALC
#include <gint/keyboard.h>
#else
#include "shared/locals.h"
#include <iostream>
#endif // DEST_CASIO_CALC

#include <cstring>
#include <cstdlib>
#include <ctime>

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

// App. infos
//
#define APP_NAME     "sudoSolver"
#define APP_VERSION  "0.4.6"
#define APP_AUTHOR   "GeeHB"

// Grids folder
//
#ifdef DEST_CASIO_CALC
#define CHAR_PATH_SEPARATOR  '\\'
#define PATH_SEPARATOR  "\\"
#define GRIDS_FOLDER    "\\\\fls0\\grids"
#endif // DEST_CASIO_CALC

#define GRID_FILE_EXT               ".txt"
#define GRID_FILE_SEARCH_PATTERN    "*.txt"

// Informations about the grid
//
#define ROW_COUNT   9
#define LINE_COUNT  ROW_COUNT

#define INDEX_MIN   0
#define INDEX_MAX   (ROW_COUNT * LINE_COUNT - 1)

#define VALUE_MIN   1
#define VALUE_MAX   9

// Blinking
//
#define BLINK_TICK_DURATION     100 //  in ms
#define BLINK_TICKCOUNT         4   // "duration" of blinking in ticks

// Files
//
#define VALUE_SEPARATOR ','     // Value separator in files

#ifndef GRIDS_FOLDER
#define GRIDS_FOLDER            u"\\\\fls0\\grids"
#endif // #ifdef GRID_FOLDER

#define FILE_LINE_SIZE  ROW_COUNT * 2   // (value & separator) * ROW_COUNT
#define FILE_SIZE       LINE_COUNT * FILE_LINE_SIZE

// Grid's solution(s)
//

// Window size & pos.
#define WIN_SOL_Y            30
#define WIN_SOL_W            200
#define WIN_SOL_H            70

// Comments
#define STR_NO_SOL          "No solution"
#define STR_ONE_SOL         "A unique sol."
#define STR_MULTIPLE_SOL    "More than one sol."

// Display
//

// Dimensions in pixels
#define GRID_HORZ_OFFSET        20
#define SQUARE_SIZE             20
#define INT_SQUARE_SIZE         (SQUARE_SIZE - 2 * BORDER_THICK)
#define GRID_SIZE               SQUARE_SIZE * ROW_COUNT
#define BORDER_THICK            2   // Thickness of external border

#define TEXT_X                  (2*GRID_HORZ_OFFSET + GRID_SIZE)
#define TEXT_Y                  30
#define TEXT_V_OFFSET           20

// Hypotheses
//
#define HYP_SQUARE_SIZE         5   // Square over lement

// List
#define HYP_LIST_SIZE           (HYP_SQUARE_SIZE + 1)
#define HYP_LIST_X              
#define HYP_LIST_Y

// Colours
//
#define EXT_BORDER_COLOUR       C_RGB(10, 19, 23)   // Large ext. borders
#define INT_BORDER_COLOUR       C_RGB(12, 12, 31)   // Thin int. borders
#define SCREEN_BK_COLOUR        C_RGB(28, 28, 30)
#define GRID_BK_COLOUR          C_RGB(24, 24, 26)
#define GRID_BK_COLOUR_DARK     C_RGB(21, 21, 22)
#define TXT_COLOUR              C_RGB(8, 8, 8)
#define TXT_ORIGINAL_COLOUR     C_RGB(30, 15, 14)
#define TXT_OBVIOUS_COLOUR      EXT_BORDER_COLOUR
#define SEL_BK_COLOUR           C_RGB(6, 6, 31)
#define SEL_TXT_GAME_COLOUR     TXT_COLOUR
//
#ifdef DEST_CASIO_CALC
enum GAME_KEY{
    KEY_CODE_F1 = KEY_F1,     // !!!
    KEY_CODE_F6 = KEY_F6,
    KEY_CODE_UP = KEY_UP,
    KEY_CODE_DOWN = KEY_DOWN,
    KEY_CODE_LEFT = KEY_LEFT,
    KEY_CODE_RIGHT = KEY_RIGHT,
    KEY_CODE_0 = KEY_0,
    KEY_CODE_1 = KEY_1,
    KEY_CODE_2 = KEY_2,
    KEY_CODE_3 = KEY_3,
    KEY_CODE_4 = KEY_4,
    KEY_CODE_5 = KEY_5,
    KEY_CODE_6 = KEY_6,
    KEY_CODE_7 = KEY_7,
    KEY_CODE_8 = KEY_8,
    KEY_CODE_9 = KEY_9,
    KEY_CODE_CAPTURE = KEY_7,
    KEY_CODE_EXIT = KEY_EXIT,
    KEY_CODE_EXE = KEY_EXE
};
#endif // #ifdef DEST_CASIO_CALC

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

#endif // __S_SOLVER_CONSTANTS_h__

// EOF
