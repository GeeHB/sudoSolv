//----------------------------------------------------------------------
//--
//--    sudoku.h
//--
//--        Definition of sudoku object - Edition and resolution
//--        of a sudoku grid
//--
//----------------------------------------------------------------------

#ifndef __S_SOLVER_SUDOKU_h__
#define __S_SOLVER_SUDOKU_h__    1

#include "consts.h"

#include "element.h"
#include "position.h"
#include "tinySquare.h"

#include "shared/bFile.h"

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

//   sudoku : Edition and/or resolution of a single sudoku grid
//
class sudoku{
public:

    // Construction & destruction
    sudoku(RECT* scr = NULL);
    ~sudoku(){}

    // display() : Display the grid and it's content
    //
    //  @update : update screen ?
    //
    void display(bool update = true);

    // empty() : Empties the grid
    //
    void empty();

    // revert : Return to the original state
    //
    void revert();

    // io
    //

    // load() : Load a new grid
    //
    //  @fName : file to load
    //
    //  @return : 0 on success or an error code
    //
    uint8_t load(const FONTCHARACTER fName);

   // save() : Save the grid on a file
    //
    //  In the given grid, only "original" values
    //  will be saved
    //
    //  @fName : File name to use
    //
    //  @return : 0 on success or an error code
    //
    int save(const FONTCHARACTER fName);

#ifdef DEST_CASIO_CALC
    // edit() : Edit / modify the current grid
    //
    //  @return : true if grid has been modified or false if unchanged
    //
    bool edit();
#endif // #ifdef DEST_CASIO_CALC

    // findObviousValues() : Find all the obvious values
    //
    //  @return : #obvious values found
    //
    uint8_t findObviousValues();

    // resolve() : Find a solution for the current grid
    //
    //  mDuration : points to an int that will receive duration
    //              of solving process in ms. Can be NULL
    //
    //  @return : true if a solution was found
    //
    bool resolve(int* mDuration = NULL);

private:

    //
    // Checks
    //

    // _checkValue() : Can we put the value at the current position ?
    //
    //  @pos : position
    //  @value : Check the given value at this 'position'
    //
    //  @return : true if the given value is valid at the given position
    //
    bool _checkValue(position& pos, uint8_t value);

    // _checkLine() : Can we put the value at the current position ?
    //
    //  @pos : position
    //  @value : Check this value at this position's line
    //
    //  @return : true if the given value is valid in the given line
    //
    bool _checkLine(position& pos, uint8_t value);

    // _checkRow() : Can we put the value at the current row ?
    //
    //  @pos : position
    //  @value : Check this value at this position's row
    //
    //  @return : true if the given value is valid in the given row
    //
    bool _checkRow(position& pos, uint8_t value);

    // _checTinySquare() : Can we put the value in this tinySquare ?
    //
    //  @pos : position
    //  @value : Check this value
    //
    //  @return : true if the given value is valid in the tinySquare
    //
    bool _checkTinySquare(position& pos, uint8_t value){
        return (false == tSquares_[pos.squareID()].inMe(elements_, value));
    }

    // _checkAndSet() : Try to  put the value at the current position
    //
    //  @pos : position
    //  @value : value to put
    //
    //  @return : true if value is set
    //
    bool _checkAndSet(position& pos, uint8_t value);

    //
    // Drawings
    //

#ifdef DEST_CASIO_CALC
    // _drawBackground() : Draw background and the grid's borders
    //
    void _drawBackground();

    // _drawContent() : : Draw all the elements
    //
    void _drawContent();

    // _drawSingleElement : draw a single element of the grid
    //
    //  @row, @line : coordinate of the element in the grid
    //  @value : value of the element in [1..9]
    //  @bkColour : background colour
    //  @txtColour : text colour
    //
    void _drawSingleElement(uint8_t row, uint8_t line, uint8_t value,
							int bkColour, int txtColour);
#endif // #ifdef DEST_CASIO_CALC

    //
    //   Search for obvious values
    //

    // _findObviousValues() :
    //  Search and set all the possible obvious values in the grid
    //
    //  @return the # of values found (and set)
    //
    uint8_t _findObviousValues();

    // _checkObviousValue() : Is there an obvious value
    //						  for the given position ?
    //
    //  pos : current position in the grid
    //
    //  @returns the value or 0
    //
    uint8_t _checkObviousValue(position& pos);

    // _setObviousValueInLines() : Try to put the value in another line
    //
    //  @pos : start position
    //  @value : value to "put"
    //
    //  @return : count (0 or 1) of value set
    //
    uint8_t _setObviousValueInLines(position& pos, uint8_t value);

    // _setObviousValueInRows() : Try to put the value in another row
    //
    //  @pos : start position
    //  @value : value to "put"
    //
    //  @return : count (0 or 1) of value set
    //
    uint8_t _setObviousValueInRows(position& pos, uint8_t value);

    //
    // Resolving
    //

    // _findFirstEmptyPos() : Find the first empty pos.
    //
    //  @start : position where to start the search
    //
    //  @return : status of position (POS_VALID or POS_END_OF_LIST)
    //
    uint8_t _findFirstEmptyPos(position &start);

    // _previousPos() : Returns to the previous position
    //
    //  Go backward in the grid to find a valid position.
    //  If position index is -1, the method will return POS_INDEX_ERROR
    //  ie. no solution for this grid
    //
    //  @current : current position
    //
    //  @return the status of the position (POS_VALID or POS_INDEX_ERROR)
    //
    uint8_t _previousPos(position& current);

#ifdef DEST_CASIO_CALC
    // __callbackTick() : Call back function for timer
    // This function is used during edition to make selected item blink
    //
    //  @pTick : pointer to blinking state indicator
    //
    //  @return : TIMER_CONTINUE if valid
    //
    static int __callbackTick(volatile int *pTick);
#endif // #ifdef DEST_CASIO_CALC

    // Members
private:
    element elements_[LINE_COUNT * ROW_COUNT];	// grid as a one dim. table
    tinySquare  tSquares_[TINY_COUNT];	// Elements IDs in tinySquares

    RECT screen_; // Position & dims of screen : {x Grid, yGrid, "screen" width , "screen" height}
};

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

#endif // __S_SOLVER_SUDOKU_h__

// EOF
