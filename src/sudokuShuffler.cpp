//----------------------------------------------------------------------
//--
//--    sudokuShuffler.cpp
//--
//--        Implementation of sudokuShuffler object
//--        Shuffle an existing sudoku to create a new one
//--
//----------------------------------------------------------------------

#include "sudokuShuffler.h"
#include "position.h"

#include <cstdlib>
#include <ctime>

// Construction
//
sudokuShuffler::sudokuShuffler(element* source){
    srand((unsigned int)clock());
    elements_ = source;
}

// shuffleValues() : randomly shuffle elements' values
//
void sudokuShuffler::shuffleValues(){
    for (uint8_t first(1); first <= VALUE_MAX; first++) {
        _swapValues(first, 1 + rand() % VALUE_MAX);
    }
}

// shuffleColumns() : randomly shuffle 2 columns in the same block
//
void sudokuShuffler::shuffleColumns(){
    uint8_t colOff(0);
    for (uint8_t block(0); block<3; block++){
        for (uint8_t colID(0); colID<3; colID++){
            _swapColumns(colID + colOff, rand() % 3 + colOff);
        }
        colOff+=3;  // Next block
    }
}

// shuffleColumnBlocks() : randomly shuffle 2 blocks of 3 columns
//
void sudokuShuffler::shuffleColumnBlocks(){
    for (int blockID(0); blockID < 3; blockID++) {
        _swapColumnBlocks(blockID, rand() % 3);
    }
}

// shuffleRows() : randomly shuffle 2 rows in the same block
//
void sudokuShuffler::shuffleRows() {
    uint8_t rowOff(0);
    for (uint8_t block(0); block<3; block++){
        for (uint8_t rowID(0); rowID<3; rowID++){
            _swapRows(rowID + rowOff, rand() % 3 + rowOff);
        }
        rowOff+=3;  // Next block
    }
}

// shuffleRowBlocks() : randomly shuffle 2 blocks of 3 rows
//
void sudokuShuffler::shuffleRowBlocks(){
    for (int blockID(0); blockID < 3; blockID++) {
        _swapRowBlocks(blockID, rand() % 3);
    }
}

//
// Internal methods
//

// _swapValues() : Swap 2 numbers in the whole grid
//
//  @first : value to replace by @second
//  @second : value to replace by @first
//
void sudokuShuffler::_swapValues(uint8_t first, uint8_t second){
    if (first != second){
        for (uint8_t index(INDEX_MIN); index <= INDEX_MAX; index++){
            if (elements_[index] == first){
                elements_[index] = second;
            }
            else{
                if (elements_[index] == second){
                    elements_[index] = first;
                }
            }
        }
    }
}

// _swapColumns() : Swap the elements of 2 columns
//
//  @fCol : col ID to swap with @sCol
//  @sCol : col ID to swap with @fcol
//
void sudokuShuffler::_swapColumns(uint8_t fCol, uint8_t sCol){
    if (fCol != sCol){
        position first, second;
        uint8_t oValue;

        first.moveTo(0, fCol);
        second.moveTo(0, sCol);

        for (uint8_t line(0); line < LINE_COUNT; line++){
            oValue = elements_[first].value();
            elements_[first] = elements_[second];
            elements_[second] = oValue;

            // Next line
            first.incLine();
            second.incLine();
        }
    }
}

// _swapColumnBlocks() : Swap blocks of 3 contiguous columns
//
void sudokuShuffler::_swapColumnBlocks(uint8_t fColBlock, uint8_t sColBlock){
    if (fColBlock != sColBlock){
         for (int colID(0); colID < 3; colID++) {
            _swapColumns(fColBlock * 3 + colID, sColBlock * 3 + colID);
        }
    }
}

// _swapRows() : Swap the elements of 2 rows
//
//  @fRow : row ID to swap with @sRow
//  @sRow : row ID to swap with @fRow
//
void sudokuShuffler::_swapRows(uint8_t fRow, uint8_t sRow){
    if (fRow != sRow){
        position first, second;
        uint8_t oValue;

        first.moveTo(fRow, 0);
        second.moveTo(sRow, 0);

        for (uint8_t row(0); row < ROW_COUNT; row++){
            oValue = elements_[first].value();
            elements_[first] = elements_[second];
            elements_[second] = oValue;

            // Next row
            first.incRow();
            second.incRow();
        }
    }
}

// _swapRowBlocks() : Swap blocks of 3 contiguous rows
//
void sudokuShuffler::_swapRowBlocks(uint8_t fRowBlock, uint8_t sRowBlock){
    if (fRowBlock != sRowBlock){
         for (int rowID(0); rowID < 3; rowID++) {
            _swapRows(fRowBlock * 3 + rowID, sRowBlock * 3 + rowID);
        }
    }
}

// EOF
