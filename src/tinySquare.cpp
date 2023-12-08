//---------------------------------------------------------------------------
//--
//--    tinySquare.cpp
//--
//--        Implementation of tinySquare object
//--          A tiny-square is one of the 9 3x3 matrix composing the whole grid
//--
//---------------------------------------------------------------------------

#include "tinySquare.h"
#include "position.h"

// Top-left index of tiny-squares
//
uint8_t gTinySquaresIndexes[] = {0, 3, 6, 27, 30, 33, 54, 57, 60};

// copyy() : Copy a tinySquare
//
//  other : tinySquate to copy
//
void tinySquare::copy(tinySquare& other){
    id_ = other.id_;
    topLine_ = other.topLine_;
    topRow_ = other.topRow_;
    _indexes();
}

// setIndex() : set index of tinySquare
//
//  @index : tiny square index
//
//  @return : true if index is valid
//
bool tinySquare::setIndex(uint8_t index){
    if (index >= (TINY_LINE_COUNT * TINY_ROW_COUNT)){
        return false;
    }

    id_ = index;

    // "top" values
    position pos(gTinySquaresIndexes[index]);
    topLine_ = pos.line();
    topRow_ = pos.row();

    // Get elements indexes
    _indexes();

    return true;
}

// findValue() : Search for the position of the value "in" the square
//
//  @elements : Game matrix
//  @value : value to search for
//  @where : pointer to the position of the value (can be NULL)
//
//   @return : a CPOINT struct {line, row} if valid or {-1, -1} if not found
//
CPOINT tinySquare::findValue(element* elements, uint8_t value, position* where){
    // Check all the positions
    for (uint8_t line = 0; line < TINY_LINE_COUNT; line++){
        for (uint8_t row = 0; row < TINY_ROW_COUNT; row++){
            if (value == elements[ids_[line][row]].value()){
                // This value is in the square
                if (where){
                    where->moveTo(line, row);
                }
                return {(int8_t)line, (int8_t)row};
            }
        }
    }

    // No, this value is not in this square
    return {-1, -1};
}

// _indexes() : Indexes matrix for the tinySquare
//
//   @ids : a 3x3 matrix containing all indexes
//
void tinySquare::_indexes(){
    uint8_t index = gTinySquaresIndexes[id_];   // start index
    for (uint8_t line = 0; line < TINY_LINE_COUNT; line++){
        for (uint8_t row = 0; row < TINY_ROW_COUNT; row++){
            ids_[line][row] = (index + row);
        }
        index+=ROW_COUNT;   // next line
    }
}
// EOF
