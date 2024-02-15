//----------------------------------------------------------------------
//--
//--    sudokuShuffler.h
//--
//--        Definition of sudokuShuffler object
//--        Shuffle an existing sudoku to create a new one
//--
//----------------------------------------------------------------------

#ifndef __SUDOKU_SHUFFLER_h__
#define __SUDOKU_SHUFFLER_h__    1

#include "element.h"

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

//   sudoku : Edition and/or resolution of a single sudoku grid
//
class sudokuShuffler{
public:

    // Construction
    sudokuShuffler(element* source);

    // Destruction
    ~sudokuShuffler(){}

    // shuffleValues() : randomly shuffle elements' values
    //
    void shuffleValues();

    // shuffleColumns() : randomly shuffle 2 columns in the same block
    //
    void shuffleColumns();

    // shuffleColumnBlocks() : randomly shuffle 2 blocks of 3 columns
    //
    void shuffleColumnBlocks();

    // shuffleRows() : randomly shuffle 2 rows in the same block
    //
    void shuffleRows();

    // shuffleRowBlocks() : randomly shuffle 2 blocks of 3 rows
    //
    void shuffleRowBlocks();

protected:

    // _swapValues() : Swap 2 values in the whole grid
    //
    //  @first : value to replace by @second
    //  @second : value to replace by @first
    //
    void _swapValues(uint8_t first, uint8_t second);

    // _swapColumns() : Swap the elements of 2 columns
    //
    //  @fCol : col ID to swap with @sCol
    //  @sCol : col ID to swap with @fcol
    //
    void _swapColumns(uint8_t fCol, uint8_t sCol);

    // _swapColumnBlocks() : Swap blocks of 3 contiguous columns
    //
    void _swapColumnBlocks(uint8_t fColBlock, uint8_t sColBlock);

    // _swapRows() : Swap the elements of 2 rows
    //
    //  @fRow : row ID to swap with @sRow
    //  @sRow : row ID to swap with @fRow
    //
    void _swapRows(uint8_t fRow, uint8_t sRow);

    // _swapRowBlocks() : Swap blocks of 3 contiguous rows
    //
    void _swapRowBlocks(uint8_t fRowBlock, uint8_t sRowBlock);

    // Members
private:
    element* elements_;
};

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

#endif // __SUDOKU_SHUFFLER_h__

// EOF
