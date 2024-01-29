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
    hypotheseColour_ = HYP_COLOUR_YELLOW; // No hypothese by default
}

// setValue() : Set or modify a value
//
//  @value : new value for the object
//  @ status : new status
//  @editMode : true if the grid is currently in edit mode
//
void element::setValue(uint8_t value, uint8_t status, bool editMode){
    if (!editMode){
        if (!_isStatusBitSet(STATUS_ORIGINAL)){
            if (value != EMPTY_VALUE){
                value_ = value;
                status_ = STATUS_SET;

                if (status != STATUS_EMPTY){
                    status_ |= status;
                }
            }
            else{
                status_ = STATUS_EMPTY;
            }
        }
    }
    else{
        value_ = value;
        status_ = (value <= 0 || STATUS_EMPTY == status )?
                    STATUS_EMPTY : STATUS_SET | STATUS_ORIGINAL;
    }
}

// empty() : Empty the element's value
//
//  @return : previous value
//
uint8_t element::empty(){
    status_ = STATUS_EMPTY;
    uint8_t value(value_);
    value_ = EMPTY_VALUE;
    hypotheseColour_ = HYP_COLOUR_NONE;
    return value;
}

// EOF
