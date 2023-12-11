//---------------------------------------------------------------------------
//--
//--    sudoku.cpp
//--
//--        Implementation of sudoku object - Edition and resolution of a
//--        sudoku grid
//--
//---------------------------------------------------------------------------

#include "sudoku.h"
#include "shared/bFile.h"
#include "shared/keyboard.h"

#include <math.h>

// Construction
//
sudoku::sudoku(){
    // Initializes tinySquares
    for (uint8_t index=0; index<TINY_COUNT; index++){
        tSquares_[index].setIndex(index);
    }

    empty();    // Start with an empty grid

    /*
    // basic grid for tests
#ifdef DEST_CASIO_CALC
    position pos(0, false);
    for (uint8_t i=0; i < 9; i++){
        elements_[pos].setValue(i+1, STATUS_ORIGINAL);
        pos.forward(10);    // pos += 10 (on remplit la diagonale)
    }
#endif // #ifdef DEST_CASIO_CALC
    */
}

// display() : Display the grid and it's content
//
void sudoku::display(){
#ifdef DEST_CASIO_CALC
    _drawBorders();
    _drawContent();
    dupdate();
#else
    position pos(0, false);
    element* pElement(NULL);
    char car;
    for (uint8_t line = 0; line < LINE_COUNT; line++){
        cout << "\t";

        for (uint8_t row = 0; row < ROW_COUNT; row++){
            pElement = &elements_[pos];
            car = '0';
            if (!pElement->isEmpty()){
                car += pElement->value();
            }

            cout << car;
            if (row == (ROW_COUNT-1)){
                cout << endl;
            }
            else{
                cout << ',';
            }

            pos+=1;  // next position
        }
    }
#endif // #ifdef DEST_CASIO_CALC
}

// empty() : Empties the grid
//
void sudoku::empty(){
    for (uint8_t index=0; index <= INDEX_MAX ; index++){
        elements_[index].empty();
    }
}

// revert : Return to the original state
//
void sudoku::revert(){
    for (uint8_t index=0; index <= INDEX_MAX ; index++){
        if (!elements_[index].isOriginal()){
            elements_[index].empty();
        }
    }
}

// load() : Load a new grid
//
//  @fName : file to load
//
//  @return : 0 on success or an error code
//
uint8_t sudoku::load(FONTCHARACTER fName){
    if (!fName || !fName[0]){
        return SUDO_NO_FILENAME;
    }

    // Load the grid
    bFile iFile;
    char buffer[FILE_SIZE+1];

    if (!iFile.open(fName, BFile_ReadOnly)){
        return SUDO_IO_ERROR;   // Unable to open the file
    }

    if (FILE_SIZE != iFile.read(buffer, FILE_SIZE, 0)){
        return SUDO_INVALID_FILESIZE;
    }

    iFile.close();
    empty();    // restart with an empty grid

    // Parse the buffer
    position pos(INDEX_MIN, false);  // Begining ot matrix
    char car;
    uint8_t value, index(0);
    while (index < FILE_SIZE){
        car = buffer[index++]; // "value"
        if (car >= '0' and car <= '9'){
            value = (uint8_t)(car - '0');
            if (value && _checkValue(pos, value)){
                elements_[pos].setValue(value, STATUS_ORIGINAL);   // This value is valid at this position and is ORIGINAL
            }
        }
        else{
            return SUDO_INVALID_FORMAT;
        }

        // separator
        car = buffer[index++];

        // next element
        pos+=1;
    }

    // The new grid is valid
    return SUDO_NO_ERROR;
}

// save() : Save the grid on a file
//
//  @fName : Name to use
//
//  @return : 0 on success or an error code
//
uint8_t sudoku::save(FONTCHARACTER fName){
    if (!fName || !fName[0]){
        return SUDO_NO_FILENAME;   // No valid file name
    }

    // Transfer content in a buffer
    char buffer[FILE_SIZE+1];
    element* pElement(NULL);
    position pos(INDEX_MIN, false);  // Begining ot matrix
    int index(0);
    uint8_t lId, cId;
    for (lId = 0; lId < LINE_COUNT; lId++){
        for (cId = 0; cId < ROW_COUNT; cId++){
            pElement = &elements_[pos];
            buffer[index++] = pElement->isEmpty()?'0':('0' + pElement->value());    // '0' means empty !
            buffer[index++] = VALUE_SEPARATOR;

            pos+=1;  // next element
        }

        // Replace separator by LF
        buffer[index-1] = '\n';
    }

    buffer[FILE_SIZE] = '\0';   // for trace purpose

    // Save the file
    bFile oFile;
    int fSize(FILE_SIZE);
    oFile.remove(fName);    // Remove the file (if already exist)

    if (!oFile.create(fName, BFile_File, &fSize)){
        return SUDO_IO_ERROR;
    }

    // copy the buffer to the file
    bool done = oFile.write(buffer, FILE_SIZE);
    oFile.close();

    // Done ?
    return (done?SUDO_NO_ERROR:SUDO_IO_ERROR);
}

#ifdef DEST_CASIO_CALC

// edit() : Edit / modify the current grid
//
//  @return : true if grid has been modified or false if left unchanged
//
bool sudoku::edit(){
    bool modified(false);
    bool cont(true);
    uint car(0);
    position currentPos(0, false);
    position prevPos(0, false);
    keyboard keys;

    while (cont){
        // if sel. changed, erase previously selected element
        if (prevPos != currentPos){
            _drawSingleElement(prevPos.row(), prevPos.line(), elements_[prevPos].value(), BK_COLOUR, ORIGINAL_COLOUR);
        }

        // Hilight the new value
        _drawSingleElement(currentPos.row(), currentPos.line(), elements_[currentPos].value(), SEL_BK_COLOUR, SEL_TXT_COLOUR);

        dupdate();
        prevPos = currentPos;

        // Wait for a keyboard event
        car = keys.getKey();

        switch (car){
        // Change the cursor's position
        //
        case KEY_CODE_LEFT:
            currentPos.decRow();    // one row left
            break;

        case KEY_CODE_RIGHT:
            currentPos.incRow();    // one row right
            break;

        case KEY_CODE_UP:
            currentPos.decLine();   // one line up
            break;

        case KEY_CODE_DOWN:
            currentPos.incLine();   // one line down
            break;

        // Change the current value
        //
        case KEY_CODE_0:
            elements_[currentPos].empty();
            modified = true;
            break;

        case KEY_CODE_1:
            modified = _checkAndSet(currentPos, 1);
            break;

        case KEY_CODE_2:
            modified = _checkAndSet(currentPos, 2);
            break;

        case KEY_CODE_3:
            modified = _checkAndSet(currentPos, 3);
            break;

        case KEY_CODE_4:
            modified = _checkAndSet(currentPos, 4);
            break;

        case KEY_CODE_5:
            modified = _checkAndSet(currentPos, 5);
            break;

        case KEY_CODE_6:
            modified = _checkAndSet(currentPos, 6);
            break;

        case KEY_CODE_7:
            modified = _checkAndSet(currentPos, 7);
            break;

        case KEY_CODE_8:
            modified = _checkAndSet(currentPos, 8);
            break;

        case KEY_CODE_9:
            modified = _checkAndSet(currentPos, 9);
            break;

        // Exit from "edit" mode
        case KEY_CODE_EXE:
            cont = false;
            break;

        // ???
        default:
            break;
        } // switch (car)
    } // while (cont)

    // unselect
    _drawSingleElement(currentPos.row(), currentPos.line(), elements_[currentPos].value(), BK_COLOUR, ORIGINAL_COLOUR);

    return modified;
}

#endif // #ifdef DEST_CASIO_CALC

// findObviousValues() : Find all the obvious values
//
//  @return : #obvious values found
//
uint8_t sudoku::findObviousValues(){
    uint8_t found(0);
    uint values(0);
    do{
        values = _findObviousValues();
        found += values;
    } while(values);    // since we put some values, maybe we can guess new ones

    return found;
}

// resolve() : Find a solution for the current grid
//
//  @return : true if a solution was found
//
bool sudoku::resolve(){
    uint8_t candidate(0);
    position pos(0, true);
    uint8_t status = _findFirstEmptyPos(pos);      // Start from the first empty place

    // All the elements "before" the current position - pos - are set with possible/allowed values
    // we'll try to put the "candidate" value (ie. the smallest possible value) at the current position
    while (POS_VALID == status){
        candidate++;   // Next possible value

        if (candidate > VALUE_MAX){
            // No possible value found at this position
            // we'll have to go backward, to the last value setted
            // when no position can be found (ie. all possibles values have  been previously
            // tested), the next pos has an invalid index, -1, and status = POS_INDEX_ERROR
            // no soluton can be found
            if (POS_VALID == (status = _previousPos(pos))){
                // The next candidate value is the currently used value + 1
                candidate = elements_[pos].empty();
            }
        }
        else{
            // Try to put the "candidate" value at current position
            //
            if (_checkValue(pos, candidate)){
                // Possible => put this candidate value
                elements_[pos].setValue(candidate);

                // Go to the next "empty" position
                // if the grid is completed, the next pos is out of range ! (status = POS_END_OF_LIST)
                status = _findFirstEmptyPos(pos);

                // At the next pos., we'll use (again) the lowest possible value
                candidate = 0;
            }
        }
    } // while (!status)

    // Found a solution ?
    return (POS_END_OF_LIST == status);
}

//
// Internal methods
//

//
// Checks
//

// _checkLine() : Can we put the value at the current line ?
//
//  @pos : position
//  @value : Check this value at this position's line
//
//  @return : true if the given value is valid in the given line
//
bool sudoku::_checkLine(position& pos, uint8_t value){
    uint8_t idFirst(pos.line() * ROW_COUNT);
    for (uint row = 0; row < ROW_COUNT; row++){
        if (elements_[idFirst + row].value() == value){
            return false;   // The value is already in the line
        }
    }

    // yes !!!
    return true;
}

// _checkRow() : Can we put the value at the current row ?
//
//  @pos : position
//  @value : Check this value at this position's row
//
//  @return : true if the given value is valid in the given row
//
bool sudoku::_checkRow(position& pos, uint8_t value){
    uint8_t idFirst(pos.row());
    for (uint line = 0; line < LINE_COUNT; line++){
        if (elements_[line * ROW_COUNT + idFirst].value() == value){
            return false;  // The value is already in the row
        }
    }

    // yes
    return true;
}

// _checkValue() : Can we put the value at the current position ?
//
//  @pos : position
//  @value : Check the given value at this 'position'
//
//  @return : true if the given value is valid at the given position
//
bool sudoku::_checkValue(position& pos, uint8_t value){
    bool line = _checkLine(pos, value);
    bool row = _checkRow(pos, value);
    bool square= _checkTinySquare(pos, value);

    return (line && row && square);
}

//
//   Drawings
//

#ifdef DEST_CASIO_CALC
// _drawBorders() : Draw the grid's borders
//
void sudoku::_drawBorders(){
    // background ...
    drect(0, 0, GRID_SIZE + 2 * DELTA_X, GRID_SIZE + 2 * DELTA_Y, BK_COLOUR);

    // thin borders ...
    uint8_t pos;
    uint8_t id;
    for (id = 1; id < LINE_COUNT; id++){
        pos = DELTA_X + id * SQUARE_SIZE;
        dline(pos, DELTA_Y, pos, DELTA_Y + GRID_SIZE, BORDER_COLOUR);   // vert
        dline (DELTA_X, pos, DELTA_X + GRID_SIZE, pos, BORDER_COLOUR);  // horz
    }

    // ... large ext. borders
    uint8_t lSquare(SQUARE_SIZE * 3);
    uint8_t thick(BORDER_THICK - 1);
    for (id = 0; id <= 3; id++){
        pos = DELTA_X + id * lSquare;
        drect(pos, DELTA_Y, pos + thick, DELTA_Y + GRID_SIZE, BORDER_COLOUR);   // vert
        drect(DELTA_X, pos, DELTA_X + GRID_SIZE, pos + thick, BORDER_COLOUR);   // horz
    }
}

// _drawContent() : : Draw all the elements
//
void sudoku::_drawContent(){
    position pos(0, false);
    element* pElement(NULL);
    uint8_t line, row;
    for (line = 0; line < LINE_COUNT; line++){
        for (row = 0; row < ROW_COUNT; row++){
            pElement = &elements_[pos];
            if (!pElement->isEmpty()){
                _drawSingleElement(row, line, pElement->value(), BK_COLOUR, (pElement->isOriginal()?ORIGINAL_COLOUR:(pElement->isObvious()?OBVIOUS_COLOUR:TXT_COLOUR)));
            }
            pos+=1; // next element
        }
    }
}

// _drawSingleElement : draw a single element of the grid
//
//  @row, @line : coordinate of the element in the grid
//  @value : value of the element in [1..9]
//  @bkColour : background colour
//  @txtColour : text colour
//
void sudoku::_drawSingleElement(uint8_t row, uint8_t line, uint8_t value, int bkColour, int txtColour){
    uint8_t x(DELTA_X + row * SQUARE_SIZE + BORDER_THICK);
    uint8_t y(DELTA_Y + line * SQUARE_SIZE + BORDER_THICK);

    // Erase background
    drect(x, y, x + INT_SQUARE_SIZE, y + INT_SQUARE_SIZE, bkColour);

    // display the value (if valid)
    if (value){
        char sVal[2];
        sVal[0] = '0' + value;
        sVal[1] = '\0';

        // text dims
        int w, h;
        dsize(sVal, NULL, &w, &h);

        // Center the text
        uint8_t dx(1 + (INT_SQUARE_SIZE - w) / 2);
        uint8_t dy(1 + (INT_SQUARE_SIZE - h) / 2);
        dtext_opt(x + dx, y + dy, txtColour, bkColour, DTEXT_LEFT, DTEXT_TOP, sVal, 1);
    }
}
#endif // #ifdef DEST_CASIO_CALC

//
//   Obvious values
//

// _findObviousValues() :
//  Search and set all the possible obvious values in the grid
//
//  @return the # of values found (and set)
//
uint8_t sudoku::_findObviousValues(){
    uint8_t found(0);
    position pos(0, true);
    int8_t value;

    for (uint8_t index = 0; index <= INDEX_MAX; index++){
        if (elements_[pos].isEmpty()){
            // Try to set a single value at this empty place
            if ((value = _checkObviousValue(pos))){
                // One more obvious value !!!!
                elements_[pos].setValue(value, STATUS_OBVIOUS);
                found++;
            }
        }
        else{
            value = elements_[pos].value();    // Current "ORIGINAL" val.

            // Can we put this value on another line ?
            found += _setObviousValueInLines(pos, value);

            // ... or/and put it in another row ?
            found += _setObviousValueInRows(pos, value);
        }

        // Next pos.
        pos+=1;

    }   // for

    return found;
}

// _checkObviousValue() : Is there an obvious value for the given position ?
//
//  pos : current position in the grid
//
//  @returns the value or 0
//
uint8_t sudoku::_checkObviousValue(position& pos){
    uint8_t value(0);

    for (uint8_t candidate = VALUE_MIN; candidate <= VALUE_MAX; candidate++){
        if (_checkValue(pos, candidate)){
            // This candidate value can be used
            if (value){
                // already a possible value at this pos.
                // => no unique value can be guessed
                return 0;
            }
            value = candidate;
        }
    }

    return value;
}

// _setObviousValueInLines() : Try to put the value in another line
//
//  @pos : start position
//  @value : value to "put"
//
//  @return : count (0 or 1) of value set
//
uint8_t sudoku::_setObviousValueInLines(position& pos, uint8_t value){

    // Check for lines in the 3 tinySquares
    //

    // tinySquares IDs for this line
    uint8_t modID(pos.squareID() % 3);
    uint8_t firstID, secondID;
    if (0 ==  modID){
        // At the left pos
        firstID = pos.squareID() + 1;
        secondID = pos.squareID() + 2;
    }
    else{
        if (1 == modID){
            // centered
            firstID = pos.squareID() - 1;
            secondID = pos.squareID() + 1;
        }
        else{
            // on the right
            firstID = pos.squareID() - 2;
            secondID = pos.squareID() - 1;
        }
    }

    // Is the value already in theses tiny squares ?
    CPOINT firstPos(tSquares_[firstID].findValue(elements_, value));
    CPOINT secondPos(tSquares_[secondID].findValue(elements_, value));

    // No for both them or yes for both
    if ((-1 == firstPos.line && -1 == secondPos.line) || (-1 != firstPos.line && -1 != secondPos.line)){
        return 0;
    }

    // Just one square misses the value => we'll try to put this value in the correct line
    //
    uint8_t candidate, candidateLine;
    if (-1 == firstPos.line){
        candidate = firstID;
        candidateLine = 2 * (tSquares_[firstID].topLine() + 1) - secondPos.line - pos.line() + 1;
    }
    else{
        candidate = secondID;
        candidateLine = 2 * (tSquares_[secondID].topLine() + 1) - firstPos.line - pos.line() + 1;
    }

    // Try to put the value in the line
    //
    bool found(false);
    position candidatePos(0, true), foundPos;
    candidatePos.moveTo(candidateLine, tSquares_[candidate].topRow());   // First row ID

    for (uint8_t row = 0; row < TINY_ROW_COUNT; row++){
        if (elements_[candidatePos].isEmpty() && _checkValue(candidatePos, value)){
            // found a valid pos. in the line for the value
            if (found){
                // Already a candiate
                return 0;
            }

            foundPos.set(candidatePos);
            found = true;
        }

        // Next row
        candidatePos+=1;
    }

    // Did we find a position ?
    if (found){
        // Yes !!!
        elements_[foundPos].setValue(value, STATUS_OBVIOUS);   // One more obvious value
        return 1;
    }

    // No ...
    return 0;
}

// _setObviousValueInRows() : Try to put the value in another row
//
//  @pos : start position
//  @value : value to "put"
//
//  @return : count (0 or 1) of value set
//
uint8_t sudoku::_setObviousValueInRows(position& pos, uint8_t value){

    // Check for lines in the 3 tinySquares
    //

    // tinySquares IDs for this line
    uint8_t modID(floor(pos.squareID() / 3));
    uint8_t firstID, secondID;
    if (0 ==  modID){
        // At the top pos
        firstID = pos.squareID() + TINY_ROW_COUNT;
        secondID = pos.squareID() + 2 * TINY_ROW_COUNT;
    }
    else{
        if (1 == modID){
            // centered
            firstID = pos.squareID() - TINY_ROW_COUNT;
            secondID = pos.squareID() + TINY_ROW_COUNT;
        }
        else{
            // on the bottom
            firstID = pos.squareID() - 2 * TINY_ROW_COUNT;
            secondID = pos.squareID() - 1 * TINY_ROW_COUNT;
        }
    }

    // Is the value already in theses tiny squares ?
    CPOINT firstPos(tSquares_[firstID].findValue(elements_, value));
    CPOINT secondPos(tSquares_[secondID].findValue(elements_, value));

    // No for both them or yes for both
    if ((-1 == firstPos.row && -1 == secondPos.row) || (-1 != firstPos.row && -1 != secondPos.row)){
        return 0;
    }

    // Just one square misses the value => we'll try to put this value in the correct line
    //
    uint8_t candidate, candidateRow;
    if (firstPos.row == -1){
        candidate = firstID;
        candidateRow = 2 * (tSquares_[firstID].topRow() + 1) - secondPos.row - pos.row() + 1;
    }
    else{
        candidate = secondID;
        candidateRow = 2 * (tSquares_[secondID].topRow() + 1) - firstPos.row - pos.row() + 1;
    }

    // Try to put the value ...
    //
    bool found(false);
    position candidatePos(0, true), foundPos;
    candidatePos.moveTo(tSquares_[candidate].topLine(), candidateRow);

    for (uint8_t line = 0; line < TINY_LINE_COUNT; line++){
            if  (elements_[candidatePos].isEmpty() && _checkValue(candidatePos, value)){
                if (found){
                    // Already a candiate
                    return 0;
                }

                foundPos.set(candidatePos);
                found = true;
            }

            // Next line
            candidatePos += ROW_COUNT;
    }

    // Did we find a valid position ?
    if (found){
        // Yes !!!
        elements_[foundPos].setValue(value, STATUS_OBVIOUS);
        return 1;
    }

    // No ...
    return 0;
}

//
// Resolving
//

// _findFirstEmptyPos() : Find the first empty pos.
//
//  @start : position where to start the search
//
//  @return : status of position (POS_VALID or POS_END_OF_LIST)
//
uint8_t sudoku::_findFirstEmptyPos(position &start){
    while (POS_END_OF_LIST != start.status() && !elements_[start].isEmpty()){
        start += 1; // next element
    }

    // Done ?
    return start.status();
}

// _previousPos() : Returns to the previous position
//
//  Go backward in the grid to find a valid position.
//  If pisition index is -1, the method will return POS_INDEX_ERROR : no solution for this grid
//
//  @current : current position
//
//  @return the status of the position (POS_VALID or POS_INDEX_ERROR)
//
uint8_t sudoku::_previousPos(position& current){
    // Clear value at current post
    elements_[current].empty();

    // Don't touch "Original" nor "Obvious" values
    current -= 1;
    while (POS_VALID == current.status() && !elements_[current].isChangeable()){
        current -= 1;
    }

    return current.status();
}

// EOF
