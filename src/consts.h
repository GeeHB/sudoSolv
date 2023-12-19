//---------------------------------------------------------------------------
//--
//--    consts.h
//--
//--		App. constants
//--
//---------------------------------------------------------------------------

#ifndef __S_SOLVER_CONSTANTS_h__
#define __S_SOLVER_CONSTANTS_h__    1

#include "shared/casioCalcs.h"

#ifdef DEST_CASIO_CALC
#include <gint/keyboard.h>
#else
#include <iostream>
#include <cstdint>
using namespace std;
#endif // DEST_CASIO_CALC

#include <cstring>

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

// App. infos
//
#define APP_NAME     "sudoSolv"
#define APP_VERSION  "0.1-2"
#define APP_AUTHOR   "GeeHB"

// Informations about the grid
//
#define ROW_COUNT   9
#define LINE_COUNT  ROW_COUNT

#define INDEX_MIN   0
#define INDEX_MAX   (ROW_COUNT * LINE_COUNT - 1)

#define VALUE_MIN   1
#define VALUE_MAX   9

// Files
//
#define VALUE_SEPARATOR ','     // Value separator in files

#ifdef DEST_CASIO_CALC
#define GRID_FOLDER      u"\\\\fls0\\GRIDS"
#else
#define GRID_FOLDER     "/home/jhb/Nextcloud/personnel/JHB/dev/cpp/sudoSolv/GRIDS"
#endif // #ifdef DEST_CASIO_CALC

#define FILE_LINE_SIZE  ROW_COUNT * 2   // (value & separator) * ROW_COUNT
#define FILE_SIZE       LINE_COUNT * FILE_LINE_SIZE

// Error codes
//
#define SUDO_NO_ERROR           0
#define SUDO_INVALID_FILENAME   1       // File doesn't exist
#define SUDO_INVALID_FILESIZE   2
#define SUDO_NO_FILENAME        3
#define SUDO_IO_ERROR           4
#define SUDO_INVALID_LINE       11
#define SUDO_INVALID_FORMAT     12
#define SUDO_VALUE_ERROR        13      // The value can't be set at this position

// Display
//

// Dimensions in pixels
#define SQUARE_SIZE             20
#define INT_SQUARE_SIZE         (SQUARE_SIZE - 2 * BORDER_THICK)
#define GRID_SIZE               SQUARE_SIZE * ROW_COUNT
#define BORDER_THICK            2   // Thickness of external border

// Colors
#ifdef DEST_CASIO_CALC
#define BORDER_COLOUR           C_RGB(10, 19, 23)
#define BK_COLOUR               C_RGB(28, 28, 30)
#define TXT_COLOUR              C_RGB(8, 8, 8)
#define ORIGINAL_COLOUR         C_RGB(30, 15, 14)
#define OBVIOUS_COLOUR          BORDER_COLOUR
#define SEL_BK_COLOUR           C_RGB(6, 6, 31)
#define SEL_TXT_COLOUR          C_WHITE
#endif // #ifdef DEST_CASIO_CALC

// Keyboard
//

// Key codes
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
    KEY_CODE_EXIT = KEY_EXIT,
    KEY_CODE_EXE = KEY_EXE
};
#else
enum GAME_KEY{
    KEY_CODE_F1 = 'A',
    KEY_CODE_F6 = 'F',
    KEY_CODE_LEFT = 's',
    KEY_CODE_RIGHT = 'f',
    KEY_CODE_UP = 'e',
    KEY_CODE_DOWN = 'x',
    KEY_CODE_0 = '0',
    KEY_CODE_1 = '1',
    KEY_CODE_2 = '2',
    KEY_CODE_3 = '3',
    KEY_CODE_4 = '4',
    KEY_CODE_5 = '5',
    KEY_CODE_6 = '6',
    KEY_CODE_7 = '7',
    KEY_CODE_8 = '8',
    KEY_CODE_9 = '9',
    KEY_CODE_EXIT = 'q',
    KEY_CODE_EXE = '\13'
};
#endif // #ifdef DEST_CASIO_CALC

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

#endif // __S_SOLVER_CONSTANTS_h__

// EOF
