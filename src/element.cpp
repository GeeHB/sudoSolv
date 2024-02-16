//----------------------------------------------------------------------
//--
//--    element.cpp
//--
//--        Implementation of element object : A single sudoku element
//--
//----------------------------------------------------------------------

#include "element.h"

// Construction
//
element::element(int value){
    status_ = (EMPTY_VALUE == value)?
                    STATUS_EMPTY:STATUS_ORIGINAL | STATUS_SET;
    value_ = value;
    hypotheseColour_ = HYP_NO_COLOUR; // No hypothese by default
}

//
// Overloads
//

// Assignments
//
element& element::operator=(const element& right){
    status_ = right.status_;
    value_ = right.value_;
    hypotheseColour_ = right.hypotheseColour_;
    return *this;
}

element& element::operator=(uint8_t right){
    value_ = right;
    return *this;
}

// setValue() : Set or modify a value
//
//  @value : new value for the object
//  @editMode : true if the grid is currently in edit mode
//
void element::setValue(uint8_t value, bool editMode){
    if (!editMode){
        if (!_isStatusBitSet(STATUS_ORIGINAL)){
            if (value != EMPTY_VALUE){
                value_ = value;
                status_ = STATUS_SET;
            }
            else{
                status_ = STATUS_EMPTY;
            }
        }
    }
    else{
        value_ = value;
        status_ = (EMPTY_VALUE == value)?
                    STATUS_EMPTY : STATUS_SET | STATUS_ORIGINAL;
    }
}

// empty() : Empty the element
//
//  @return : previous value
//
uint8_t element::empty(){
    status_ = STATUS_EMPTY;
    uint8_t value(value_);
    value_ = EMPTY_VALUE;
    hypotheseColour_ = HYP_NO_COLOUR;
    return value;
}

// EOF
