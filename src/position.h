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
    position(uint8_t index = INDEX_MIN, bool gameMode = true);

    position(position& other){
        set(other);
    }

    // Destruction
    ~position(){}

    // Get sthe status
    //
    uint8_t status(){
        return status_;
    }

    position& operator=(const position & other){
        set(other);
        return *this;
    }

    // set() : Copy a position
    //
    //  other : position to copy
    //
    void set(const position& other);

    bool operator!=(const position& other){
        return (index_ != other.index_);
    }

    // moveTo() : Move to an absolute position
    //
    //  @line, @row : new absolute position
    //
    void moveTo(uint8_t line, uint8_t  row);


    // Index (ie. absolute value)
    //

    // implicit conversion
    operator uint8_t() const{
        return index_;
    }

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

    position& operator+=(uint8_t inc){
        forward(inc);
        return *this;
    }
    uint8_t forward(uint8_t inc = 1);

    position& operator-=(uint8_t dec){
        backward(dec);
        return *this;
    }
    uint8_t backward(uint8_t dec = 1);

    // Change "row"
    //
    void decRow(uint8_t dec = 1);
    void incRow(uint8_t inc = 1);

    // Change "line"
    //
    void decLine(uint8_t dec = 1);
    void incLine(uint8_t inc = 1);

private:
    // _whereAmI() : Updating coordinates according to (new) index
    //
    //  @all :  if true, line,row and squareIndex are updated
    //          if false, only square index
    //
    void _whereAmI(bool all = true);

    // _setInRange() : Set (force) a vlue in the col range
    //
    //  @value : value to check
    //
    //  @return : value in the range [0, ROW_COUNT]
    //
    uint8_t _setInRange(int8_t value);

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
