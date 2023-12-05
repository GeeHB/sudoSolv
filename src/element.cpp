
//---------------------------------------------------------------------------
//--
//--    element.cpp
//--
//--		Implementation of element object : A single sudoku element
//--
//---------------------------------------------------------------------------

#include "element.h"

// setValue() : Set or modify a value
//
//  @value : new value for the object
//  @ status : new status
//  @editMode : true if the grid is currentlin edit mode
//
void element::setValue(int value, uint8_t status, bool editMode){
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
        status_ = (value <= 0 || STATUS_EMPTY == status )?STATUS_EMPTY : STATUS_SET | STATUS_ORIGINAL;
    }
}

// EOF
