//----------------------------------------------------------------------
//--
//--    element.h
//--
//--        Definition of element object : A single sudoku element
//--
//----------------------------------------------------------------------

#ifndef __S_SOLVER_ELEMENT_h__
#define __S_SOLVER_ELEMENT_h__    1

#include "consts.h"

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

// Element status
//
#define STATUS_EMPTY    0
#define STATUS_SET      1
#define STATUS_OBVIOUS  2   // An obvious value found at runtime
#define STATUS_ORIGINAL 4   // Can't be changed (except in edition mode)

// An empty element value
#define EMPTY_VALUE     0

// element - A single sudoku grid element
//
class element{

public:

    // Construction
    element(int value = EMPTY_VALUE){
        status_ = (EMPTY_VALUE == value)?
                        STATUS_EMPTY:STATUS_ORIGINAL | STATUS_SET;
        value_ = value;
    }

    // Destrcution
    ~element(){}

    // setValue() : Set or modify a value
    //
    //  @value : new value for the object
    //  @ status : new status
    //  @editMode : true if the grid is currently in edit mode
    //
    void setValue(uint8_t value, uint8_t status = STATUS_EMPTY,
                    bool editMode = false);

    // Access
    //
    uint8_t value(){
        return (_isStatusBitSet(STATUS_SET)?value_:EMPTY_VALUE);
    }

    // empty() : Empty the element's value
    //  @return : previous value
    uint8_t empty(){
        status_ = STATUS_EMPTY;
        uint8_t value(value_);
        value_ = EMPTY_VALUE;
        return value;
    }

    // Element's status
    //
    bool isEmpty(){
        return (STATUS_EMPTY == status_);
    }

    bool isOriginal(){
        return _isStatusBitSet(STATUS_ORIGINAL);
    }

    bool isObvious(){
        return _isStatusBitSet(STATUS_OBVIOUS);
    }

    bool isChangeable(){
        return (status_ ==  STATUS_EMPTY || status_ == STATUS_SET);
    }

private:
        // _isStatusBitSet : Check wether a bit is set
        //
        //  @singleBit : bit to check
        //
        // @return : true if the bit is present in the object's status
        //
        bool _isStatusBitSet(int singleBit){
            return ((status_ & singleBit) == singleBit);
        }

    // Members
private:
        uint8_t     status_;
        uint8_t     value_;
}; // class element

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

#endif // __S_SOLVER_ELEMENT_h__

// EOF
