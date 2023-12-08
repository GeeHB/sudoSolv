//---------------------------------------------------------------------------
//--
//--    position.cpp
//--
//--        Implementation of position object - Position in sudoku's grid
//--
//---------------------------------------------------------------------------

#include "position.h"

#include <math.h>

position::position(uint8_t index, bool gameMode){
    index_ = index;
    status_ = (index <= INDEX_MAX)?POS_VALID:POS_INDEX_ERROR;

    gameMode_ = gameMode;
    row_ = line_ = 0;

    _whereAmI();
}

// set() : Copy a position
//
//  other : position to copy
//
void position::set(const position& other){
    index_ = other.index_;
    status_ = other.status_;
    gameMode_ = other.gameMode_;
    row_ = other.row_;
    line_ = other.line_;
    squareID_ = other.squareID_;
}

// moveTo() : Move to an absolute position
//
//  @line, @row : new absolute position
//
void position::moveTo(uint8_t line, uint8_t  row){
    // Ensure position is in the grid
    row_ = _setInRange(row);
    line_ = _setInRange(line);

    index_ = row_ + line_ * ROW_COUNT;
    _whereAmI(false);
}

// Change index
//

// +=
//
uint8_t position::forward(uint8_t inc){
    index_ += inc;

    // Reach the end of the matrix ?
    if (index_ > INDEX_MAX){
        if (gameMode_){
            status_ = POS_END_OF_LIST;
        }
        else{
            if (index_ > (1+INDEX_MAX)){
                status_ = POS_INDEX_ERROR;
            }
        }
    }

    _whereAmI();
    return index_;
}

// -=
//
uint8_t position::backward(uint8_t dec){
    int8_t index = index_ - dec;
    if (index < INDEX_MIN){
        status_ = POS_INDEX_ERROR;
    }

    _whereAmI();
    return (index_ = index);
}

// Change "row"
//
void position::decRow(uint8_t dec){
    int8_t row(row_);
    row-=dec;
    index_ = (row < 0)?(line_ * ROW_COUNT - 1):(index_ - dec);

    _whereAmI();
}

void position::incRow(uint8_t inc){
    uint8_t row = row_ + inc;
    index_ = (row >= ROW_COUNT)?((line_ - 1) * ROW_COUNT + row):(index_ + inc);

    _whereAmI();
}

// Change "line"
//
void position::decLine(uint8_t dec){
    int8_t index = index_ - ROW_COUNT * dec;
    index_ = (index < INDEX_MIN)?(row_ + (ROW_COUNT - 1) * ROW_COUNT):index;

    _whereAmI();
}

void position::incLine(uint8_t inc){
    index_ += ROW_COUNT * inc;
    if (index_ > INDEX_MAX){
        index_ = row_;
    }

    _whereAmI();
}

// Change the value
//
uint8_t position::incValue(uint8_t value){
    uint8_t newVal = value + 1;
    return (newVal > VALUE_MAX)?(VALUE_MIN - 1):newVal;
}

uint8_t position::decValue(uint8_t value){
    uint8_t newVal = value - 1;
    return (newVal < (VALUE_MIN - 1))?VALUE_MAX:newVal;
}

//
//   Internal methods
//

// _whereAmI() : Updating coordinates according to (new) index
//
//  @all :  if true, line,row and squareIndex are updated
//          if false, only square index
//
void position::_whereAmI(bool all){
    if (all){
        line_ = floor(index_ / ROW_COUNT);
        row_ = index_ - ROW_COUNT * line_;
    }

    // Small square ID
    squareID_ = 3 * floor(line_ / 3) + floor(row_ / 3);
}

// _setInRange() : Set (force) a vlue in the col range
//
//  @value : value to check
//
//  @return : value in the range [0, ROW_COUNT]
//
uint8_t position::_setInRange(int8_t value){
    return (value < 0 ?0:(value >= ROW_COUNT?(ROW_COUNT - 1):value));
}

// EOF
