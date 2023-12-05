//---------------------------------------------------------------------------
//--
//--    position.h
//--
//--        Definition of position object - Position in sudoku's grid
//--
//---------------------------------------------------------------------------

#ifndef __S_SOLVER_POSITION_h__
#define __S_SOLVER_POSITION_h__    1

#include "consts.h"

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

// status of the "position"
//
enum POS_STATUS {
    POS_VALID = 0,
    POS_INDEX_ERROR = 1,    // invalid index value
    POS_END_OF_LIST = 2     // reach end of list in game mode (ie. a valid solution has been found)
};

// position - "ID" of an element in the sudoku's grid
//
//   This object does all the conversion from linear index to matrix coordinates
//
class position{

public:
    // Constructions
    position(uint8_t index = 0, bool gameMode = true);

    position(position& other){
        set(other);
    }

    // Get sthe status
    //
    uint8_t status(){
        return status_;
    }

    // set() : Copy a position
    //
    //  other : position to copy
    //
    void set(position& other);

    // moveTo() : Move to an absolute position
    //
    //  line, row : new absolute position
    //
    void moveTo(uint8_t line,uint8_t  row);

    // Access
    //
    uint8_t index(){
        return index_;
    }
    uint8_t row(){
        return row_;
    }
    uint8_t line(){
        return line_;
    }
    uint8_t squareID(){
        return squareID_;
    }

    // Change index
    //

    // +=
    uint8_t forward(uint8_t inc = 1);

    // -=
    uint8_t backward(uint8_t dec = 1);

    // Change "row"
    //
    void decRow(uint8_t dec = 1);
    void incRow(uint8_t inc = 1);

    // Change "line"
    //
    void decLine(uint8_t dec = 1);
    void incLine(uint8_t inc = 1);

    // Change the value
    //
    uint8_t incValue(uint8_t value);
    uint8_t decValue(uint8_t value);

private:
    // _whereAmI() : Updating coordinates according to (new) index
    //
    //  @all :  if true, line,row and squareIndex are updated
    //          if false, only square index
    //
    void _whereAmI(bool all = true);

    // Members
private:
    uint8_t index_;         // Current index
    uint8_t status_;

    uint8_t row_, line_;    // Position in the "matrix"
    uint8_t squareID_;      //tiny-square ID
    bool gameMode_;         // In game mode ?
};  // class position

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

#endif // __S_SOLVER_POSITION_h__

// EOF
