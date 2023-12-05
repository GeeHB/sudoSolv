//---------------------------------------------------------------------------
//--
//--    consts.h
//--
//--		App. constants
//--
//---------------------------------------------------------------------------

#ifndef __S_SOLVER_CONSTANTS_h__
#define __S_SOLVER_CONSTANTS_h__    1

#include <cstdint> // <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

// Information about the grid
//
#define ROW_COUNT   9
#define LINE_COUNT  9

#define INDEX_MIN   0
#define INDEX_MAX   (ROW_COUNT * LINE_COUNT - 1)

#define VALUE_MIN   1
#define VALUE_MAX   9

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

#endif // __S_SOLVER_CONSTANTS_h__

// EOF
