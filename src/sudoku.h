//---------------------------------------------------------------------------
//--
//--    sudoku.h
//--
//--        Definition of sudoku object - Edition and resolution of a
//--        sudoku grid
//--
//---------------------------------------------------------------------------

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
    sudoku();
    ~sudoku(){}

    // Display the grid and it's content
    void display();

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
    uint8_t load(FONTCHARACTER fName);

    // save() : Save the grid on a file
    //
    //  @fName : Name to use
    //
    //  @return : 0 on success or an error code
    //
    uint8_t save(FONTCHARACTER fName);

private:

    //
    // Checks
    //

    // _checkValue() : Can we put the value at the current position ?
    //
    //  @pos : position
    //  @value : Check thois value at this position
    //
    //  @return : true if the given value is valid at the given position
    //
    bool _checkValue(position& pos, uint8_t value){
        return _checkLine(pos, value) && _checkRow(pos, value) && _checkTinySquare(pos, value);
    }

    // _checkLine() : Can we put the value at the current position ?
    //
    //  @pos : position
    //  @value : Check thois value at this position's line
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

    // _checTinySquare() : Can we put the value in the current tinySquare ?
    //
    //  @pos : position
    //  @value : Check this value
    //
    //  @return : true if the given value is valid in the given tinySquare
    //
    bool _checkTinySquare(position& pos, uint8_t value){
        return (false == tSquares_[pos.squareID()].inMe(elements_, value)); // Is the value in the tinySquare ?
    }

#ifdef DEST_CASIO_CALC
    // _drawBorders() : Draw the grid's borders
    //
    void _drawBorders();

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
    void _drawSingleElement(uint8_t row, uint8_t line, uint8_t value, int bkColour, int txtColour);
#endif // #ifdef DEST_CASIO_CALC

    // Members
private:
    element elements_[LINE_COUNT * ROW_COUNT];       // grid as a one dim. table
    tinySquare  tSquares_[TINY_COUNT];              // Elements IDs in tinySquares
};

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

#endif // __S_SOLVER_SUDOKU_h__

// EOF
