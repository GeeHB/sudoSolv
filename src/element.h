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
#ifdef DEST_CASIO_CALC
enum HYP_COLOUR{
    HYP_NO_COLOUR = GRID_BK_COLOUR,
    HYP_COLOUR_YELLOW = COLOUR_YELLOW,
    HYP_COLOUR_BLUE = COLOUR_BLUE,
    HYP_COLOUR_GREEN = COLOUR_GREEN,
    HYP_COLOUR_RED = COLOUR_RED
};
#endif

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

// element - A single sudoku grid element
//
class element{

public:

    // Construction
    element(int value = EMPTY_VALUE);

    // Destrcution
    ~element(){}

    // Assignment
    element& operator=(const element& right);


    // 'Fast' access to value
    //
    element& operator=(uint8_t right);
    bool operator==(uint8_t right){
        return (value_ == right);
    }

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

    // empty() : Empty the element
    //
    //  @return : previous value
    //
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

    // setStatus() : Set element's status
    //
    //  @nStatus : element's status
    //
    void setStatus(uint8_t nStatus){
        status_ = nStatus;
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
        return (STATUS_EMPTY == status_ || STATUS_SET == status_);
    }

    //
    // Hypotheses' colours
    //

    // setHypColour() : Change the hypothese's colour
    //
    //  @hypcolour : Colour to use
    //
    void setHypColour(int hypColour){
        hypotheseColour_ = hypColour;
    }

    // hypColour() : Get the hyp. colour used by the element
    //
    //  @return : Current hyp. colour
    //
    int hypColour(){
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
