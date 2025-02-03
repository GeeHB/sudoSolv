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
#define APP_VERSION  "0.4.14"
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

#define VALUES_COUNT ROW_COUNT * LINE_COUNT

#define INDEX_MIN   0
#define INDEX_MAX   (VALUES_COUNT - 1)

#define VALUE_MIN   1
#define VALUE_MAX   ROW_COUNT

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

// Grid's solution(s) and help
//

#define WIN_SOL_TITLE       "Check"
#define WIN_GEN_TXT         "Generating..."
#define WIN_SEARCH_TXT      "Resolving in progress..."
#define WIN_FOUND_TXT       "Congratultation"

// Window size & pos.
#define WIN_SOL_Y            30
#define WIN_SOL_W            200
#define WIN_SOL_H            70

// Comments
#define STR_NO_SOL          "No solution"
#define STR_ONE_SOL         "A unique sol."
#define STR_MULTIPLE_SOL    "More than one sol."

#define MIN_CLUE_COUNT      10      // No help if fewer items to find
#define MAX_HELP_CLUES      3       // # of help clues

// Display
//

// "Internal" images
#define IMG_ABOUT_W         202
#define IMG_ABOUT_H         216     // Menu is visible !

#define IMG_PAUSE_W         CASIO_WIDTH
#define IMG_PAUSE_H         70
#define IMG_PAUSE_COPY_Y    46
#define IMG_PAUSE_LINES     (CASIO_HEIGHT - IMG_PAUSE_H)

// Grid dimensions in pixels
#define GRID_HORZ_OFFSET    20
#define SQUARE_SIZE         20
#define INT_SQUARE_SIZE     (SQUARE_SIZE - 2 * BORDER_THICK)
#define GRID_SIZE           SQUARE_SIZE * ROW_COUNT
#define BORDER_THICK        2   // Thickness of external border

// Texts (stats and hypotheses)
//
#define TEXT_V_BASE             10
#define TEXT_V_OFFSET           20

#define TEXT_BASE_X             (2*GRID_HORZ_OFFSET + GRID_SIZE)
#define TEXT_BASE_Y             (TEXT_V_OFFSET + TEXT_V_BASE)

// File
#define FILE_TEXT               "File : %s"
#define FILE_ERROR_SAVE_TEXT    "Error saving : %d"
#define FILE_ERROR_LOAD_TEXT    "Error loading : %d"
#define FILE_TEXT_X             TEXT_BASE_X
#define FILE_TEXT_Y             TEXT_BASE_Y
#define FILE_TEXT_ERROR_Y       TEXT_V_BASE

// Obvious values
#define OBV_TEXT                "%d obvious values"
#define OBV_NONE_TEXT           "No obvious value"
#define OBV_TEXT_X              TEXT_BASE_X
#define OBV_TEXT_Y              (TEXT_BASE_Y + TEXT_V_OFFSET)

// Solution
#define SOL_TEXT                "Solved in %d ms"
#define SOL_NONE_TEXT           "No solution found"
#define SOL_X                   TEXT_BASE_X
#define SOL_Y                   (TEXT_BASE_Y + 2 * TEXT_V_OFFSET)

// # values
#define VALUES_TEXT             "Val. %d / %d    "
#define VALUES_X                TEXT_BASE_X
#define VALUES_Y                (TEXT_BASE_Y + 3 * TEXT_V_OFFSET)

// Hypotheses
#define HYP_SQUARE_SIZE         5   // Square over element
#define HYP_H_OFFSET            50
#define HYP_LIST_OFFSET         4   // dx and dy

#define HYP_LIST_TEXT           "Hyp. :"
#define HYP_LIST_TEXT_X         TEXT_BASE_X
#define HYP_LIST_TEXT_Y         (TEXT_BASE_Y + 4 * TEXT_V_OFFSET)
#define HYP_LIST_X              (HYP_LIST_TEXT_X +  HYP_H_OFFSET)
#define HYP_LIST_Y              HYP_LIST_TEXT_Y
#define HYP_LIST_W              20
#define HYP_LIST_H              (HYP_SQUARE_SIZE + 1)

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
    KEY_CODE_PAUSE = KEY_OPTN,
    KEY_CODE_EXIT = KEY_EXIT,
    KEY_CODE_EXE = KEY_EXE
};
#endif // #ifdef DEST_CASIO_CALC

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

#endif // __S_SOLVER_CONSTANTS_h__

// EOF
