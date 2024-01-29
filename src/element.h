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

// Element's coloured hypothese
//
#define HYP_COLOUR_NONE COLOUR_WHITE

// element - A single sudoku grid element
//
class element{

public:

    // Construction
    element(int value = EMPTY_VALUE);

    // Destrcution
    ~element(){}

    //
    // Element's value
    //

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
    uint8_t empty();

    //
    // Element's status
    //

    // status() : Return element's status
    //
    //  @return : element's status
    //
    uint8_t status(){
        return status_;
    }

    // isEmpty() : Check wether element is empty
    //
    //  @return : true if is element empty
    //
    bool isEmpty(){
        return (STATUS_EMPTY == status_);
    }

    // isOriginal() : Is the element original
    //
    //  An original element can't be modified at runtime
    //
    //  @return : true if element is original
    //
    bool isOriginal(){
        return _isStatusBitSet(STATUS_ORIGINAL);
    }

    // isObvious() : Has the element an obvious value ?
    //
    //  An obvious value is a the only possible value
    //  at a given position
    //
    //  @return : true if element has an obvious value
    //
    bool isObvious(){
        return _isStatusBitSet(STATUS_OBVIOUS);
    }

    // isChangeable() : Can the element's value be changed
    //
    //  @return : true if element's value is changeable
    //
    bool isChangeable(){
        return (status_ ==  STATUS_EMPTY || status_ == STATUS_SET);
    }

    //
    // Hypotheses
    //

    void setHypothese();
    uint8_t hypothese(){
        return hypotheseColour_;
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
        int         hypotheseColour_;   // Colour of hypothese
}; // class element

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

#endif // __S_SOLVER_ELEMENT_h__

// EOF
