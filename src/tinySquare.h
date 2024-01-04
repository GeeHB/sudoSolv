//---------------------------------------------------------------------------
//--
//--    tinySquare.h
//--
//--        Definition of tinySquare object
//--          A tiny-square is one of the 9 3x3 matrix composing the whole grid
//--
//---------------------------------------------------------------------------

#ifndef __S_SOLVER_TSQUARES_h__
#define __S_SOLVER_TSQUARES_h__    1

#include "consts.h"
#include "element.h"
#include "position.h"

// Dimensions
#define TINY_LINE_COUNT 3
#define TINY_ROW_COUNT  TINY_LINE_COUNT

#define TINY_COUNT  9

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

typedef struct cpoint{
    int8_t line;
    int8_t row;
}CPOINT;

// tinySquare object
//   A tiny-square is one of the 9 3x3 matrix composing the whole grid
//
class tinySquare{
public:

    // Constructions
    tinySquare(){}
    tinySquare(tinySquare& other){
        // Copy
        copy(other);
    }

    // Destruction
    ~tinySquare(){}

    // copy() : Copy a tinySquare
    //
    //  other : tinySquate to copy
    //
    void copy(tinySquare& other);

    // Access
    //
    uint8_t Id(){
        return id_;
    }

    // Top indexes
    uint8_t topLine(){
        return topLine_;
    }

    uint8_t topRow(){
        return topRow_;
    }

    // setIndex() : set index of tinySquare
    //
    //  @index : tiny square index
    //
    //  @return : true if index is valid
    //
    bool setIndex(uint8_t index);

    // findValue() : Search for the position of the value "in" the square
    //
    //  @elements : Game matrix
    //  @value : value to search for
    //  @where : pointer to the position of the value (can be NULL)
    //
    //   @return : a CPOINT struct {line, row} if valid or {-1, -1} if not found
    //
    CPOINT findValue(element* elements, uint8_t value, position* where = NULL);

     // inMe() : Is the given value "in" the square ?
    //
    //   Check wether the value is already present in the current tiny-square
    //
    //  @elements : values
    //  @value : value to check
    //
    //  @return true if found
    //
    bool inMe(element* elements, uint8_t value){
        CPOINT pos = findValue(elements, value);
        return (pos.line >=0 && pos.row>=0);
    }

private:

    // _indexes() : Get elements's indexes
    //
    void _indexes();

    // Members
private:
    uint8_t id_;            // ID

    uint8_t topLine_;       // Position of top left orner of the square
    uint8_t topRow_;

    uint8_t ids_[TINY_LINE_COUNT][TINY_ROW_COUNT];   // Indexes of elements in the square
};

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

#endif // __S_SOLVER_TSQUARES_h__

// EOF

