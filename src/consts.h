//---------------------------------------------------------------------------
//--
//--    consts.h
//--
//--		App. constants
//--
//---------------------------------------------------------------------------

#ifndef __S_SOLVER_CONSTANTS_h__
#define __S_SOLVER_CONSTANTS_h__    1

#ifdef DEST_CASIO_CALC
#include <gint/display.h>
#include <gint/keyboard.h>
#else
#include <iostream>
#include <cstdint> // <stdint.h>

using namespace std;
#endif //

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

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
#define FILE_COMMENTS   '#'     // Comment lines start with

#ifdef DEST_CASIO_CALC
#define GRID_FOLDER      u"\\\\fls0\\grids"
#else
#define GRID_FOLDER     "/home/jhb/grids"
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
#ifdef DEST_CASIO_CALC
#define DELTA_X                 3   // Distance from top left corner
#define DELTA_Y                 DELTA_X // useless in firdt version ....

#define BORDER_THICK            2   // Thickness of external border

#define SQUARE_SIZE             20
#define INT_SQUARE_SIZE         (SQUARE_SIZE - 2 * BORDER_THICK)

#define GRID_SIZE               SQUARE_SIZE * ROW_COUNT

#define BORDER_COLOUR           C_RGB(10, 19, 23)
#define OBVIOUS_COLOUR          BORDER_COLOUR
#define BK_COLOUR               C_RGB(28, 28, 30)

#define TXT_COLOUR              C_RGB(8, 8, 8)
#define HILITE_COLOUR           C_RGB(30, 15, 14)

#define SEL_BK_COLOUR            C_RGB(6, 6, 31)
#define SEL_TXT_COLOUR           C_WHITE

#endif // #ifdef DEST_CASIO_CALC

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

#endif // __S_SOLVER_CONSTANTS_h__

// EOF
