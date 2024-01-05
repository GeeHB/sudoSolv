//----------------------------------------------------------------------
//--
//--    sudoku.cpp
//--
//--        Implementation of sudoku object - Edition and resolution
//--		of a sudoku grid
//--
//----------------------------------------------------------------------

#include "sudoku.h"
#include "menu.h"
#include "shared/bFile.h"
#include "shared/keyboard.h"

#include <time.h>
#include <math.h>

#ifdef DEST_CASIO_CALC
#include <gint/timer.h>
#include <gint/clock.h>
#endif // #ifdef DEST_CASIO_CALC

// Construction
//
sudoku::sudoku(RECT* scr){
    // Screen dimension
    if (scr){
        memcpy(&screen_, scr, sizeof(RECT));
    }
    else{
        screen_ = {0, 0, CASIO_WIDTH, CASIO_HEIGHT};
    }

    //screen_.x = (screen_.w - GRID_SIZE) / 2;
    screen_.x = GRID_HORZ_OFFSET;
    screen_.y = (screen_.h - GRID_SIZE) / 2;    // centered vertically

    // Initializes tinySquares
    for (uint8_t index=0; index<TINY_COUNT; index++){
        tSquares_[index].setIndex(index);
    }

    empty();    // Start with an empty grid
}

// display() : Display the grid and it's content
//
//  @update : update screen ?
//
void sudoku::display(bool update){
#ifdef DEST_CASIO_CALC
    _drawBackground();
    _drawContent();
    if (update){
        dupdate();
    }
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
uint8_t sudoku::load(const FONTCHARACTER fName){
    if (!fName || !fName[0]){
        return FILE_NO_FILENAME;
    }

    // Load the grid
    bFile iFile;
    char buffer[FILE_SIZE+1];

    if (!iFile.open(fName, BFile_ReadOnly)){
        return FILE_IO_ERROR;   // Unable to open the file
    }

    if (FILE_SIZE != iFile.read(buffer, FILE_SIZE, 0)){
        return FILE_INVALID_FILESIZE;
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
                // This value is valid at this position and is ORIGINAL
                elements_[pos].setValue(value, STATUS_ORIGINAL);
            }
        }
        else{
            return FILE_INVALID_FORMAT;
        }

        // separator
        car = buffer[index++];

        // next element
        pos+=1;
    }

    // The new grid is valid
    return FILE_NO_ERROR;
}

// save() : Save the grid on a file
//
//  In the given grid, only "original" values
//  will be saved
//
//  @fName : File name to use
//
//  @return : 0 on success or an error code
//
int sudoku::save(const FONTCHARACTER fName){
    if (!fName || !fName[0]){
        return FILE_NO_FILENAME;   // No valid file name
    }

    // Transfer content in a buffer
    char buffer[FILE_SIZE+1];
    memset(buffer, 0x00, FILE_SIZE);    // ???
    int index(0);
    for (uint8_t lId(0); lId < LINE_COUNT; lId++){
        for (uint8_t cId(0); cId < ROW_COUNT; cId++){
            // '0' means empty !
            buffer[2*index] = (elements_[index].isOriginal()?('0' + elements_[index].value()):'0');
            buffer[2*index+1] = VALUE_SEPARATOR;
            index++;
        }

        // Replace separator by LF
        buffer[2*index-1] = '\n';
    }

    buffer[FILE_SIZE] = '\0';   // for trace purpose

    // Save the file
    bFile oFile;
    int fSize(FILE_SIZE);
    oFile.remove(fName);    // Remove the file (if already exist)

    if (!oFile.create(fName, BFile_File, &fSize)){
        return oFile.getLastError();
    }

    // copy the buffer to the file
    bool done = oFile.write(buffer, FILE_SIZE);
    int error = oFile.getLastError();
    oFile.close();

    // Done ?
    return (done?FILE_NO_ERROR:error);
}

#ifdef DEST_CASIO_CALC

// edit() : Edit / modify the current grid
//
//  @return : true if grid has been modified or false if left unchanged
//
bool sudoku::edit(){
    bool modified(false);
    bool cont(true);
    position currentPos(0, false);
    position prevPos(0, false);

    revert();   // Remove obious and found values (if any)
    display();

    menuBar menu;       // A simple menu bar
    MENUACTION action;
    menu.addItem(1, IDM_EDIT_OK, IDS_EDIT_OK);
    menu.addItem(2, IDM_EDIT_CANCEL, IDS_EDIT_CANCEL);
    menu.update();

    // Initial draw
    bool showSelected(true), reDraw(false);
    _drawSingleElement(currentPos.row(), currentPos.line(),
                    elements_[currentPos].value(),
                    SEL_BK_COLOUR, SEL_TXT_COLOUR);

    // Timer for blinking effect
    int tickCount(BLINK_TICKCOUNT);
    static volatile int tick(1);
    int timerID = timer_configure(TIMER_ANY, BLINK_TICK_DURATION*1000,
					GINT_CALL(__callbackTick, &tick));
    if (timerID >= 0){
        timer_start(timerID);   // set the timer
    }

    // Edit grid
    while (cont){

        if (timerID >= 0){
            while(!tick){
                sleep();
            }
            tick = 0;

            // Time to blink ?
            if (!(tickCount--)){
                // Blink
                showSelected = !showSelected;
                tickCount = BLINK_TICKCOUNT;
                reDraw = true;
            }
        }

        // Wait for a keyboard event
        action = menu.handleKeyboard();

        switch (action.value){
        // Change the cursor's position
        //
        case KEY_CODE_LEFT:
            currentPos.decRow();    // one row left
            reDraw = true;
            break;

        case KEY_CODE_RIGHT:
            currentPos.incRow();    // one row right
            reDraw = true;
            break;

        case KEY_CODE_UP:
            currentPos.decLine();   // one line up
            reDraw = true;
            break;

        case KEY_CODE_DOWN:
            currentPos.incLine();   // one line down
            reDraw = true;
            break;

        // Change the current value
        //
        case KEY_CODE_0:
            elements_[currentPos].empty();
            modified = true;
            break;

        case KEY_CODE_1:
            if (_checkAndSet(currentPos, 1)){
                modified = true;
            }
            break;

        case KEY_CODE_2:
            if (_checkAndSet(currentPos, 2)){
                modified = true;
            }
            break;

        case KEY_CODE_3:
            if (_checkAndSet(currentPos, 3)){
                modified = true;
            }
            break;

        case KEY_CODE_4:
            if (_checkAndSet(currentPos, 4)){
                modified = true;
            }
            break;

        case KEY_CODE_5:
            if (_checkAndSet(currentPos, 5)){
                modified = true;
            }
            break;

        case KEY_CODE_6:
            if (_checkAndSet(currentPos, 6)){
                modified = true;
            }
            break;

        case KEY_CODE_7:
            if (_checkAndSet(currentPos, 7)){
                modified = true;
            }
            break;

        case KEY_CODE_8:
            if (_checkAndSet(currentPos, 8)){
                modified = true;
            }
            break;

        case KEY_CODE_9:
            if (_checkAndSet(currentPos, 9)){
                modified = true;
            }
            break;

        // Cancel modifications
        case IDM_EDIT_CANCEL:
            revert();
            cont = modified = false;
            break;

        // Exit from "edit" mode
        case KEY_MENU:
        case KEY_EXIT:
        case IDM_EDIT_OK:
            cont = false;
            break;

        // ???
        default:
            break;
        } // switch (car)

        if (reDraw || (prevPos != currentPos)){
            // if sel. changed, erase previously selected element
            if (prevPos != currentPos){
                _drawSingleElement(prevPos.row(), prevPos.line(),
                        elements_[prevPos].value(),
                        BK_COLOUR, TXT_ORIGINAL_COLOUR);
            }

            // Hilight the new value (or have it blink)
            if (showSelected){
                _drawSingleElement(currentPos.row(), currentPos.line(),
                            elements_[currentPos].value(),
                            SEL_BK_COLOUR, SEL_TXT_COLOUR);
            }
            else{
                _drawSingleElement(currentPos.row(), currentPos.line(),
                            elements_[currentPos].value(),
                            BK_COLOUR, TXT_ORIGINAL_COLOUR);
            }

            dupdate();
            prevPos = currentPos;
            reDraw = false;
        }
    } // while (cont)

    if (timerID >= 0){
        timer_stop(timerID);    // stop the timer
    }

    // unselect
    _drawSingleElement(currentPos.row(), currentPos.line(),
			elements_[currentPos].value(),
			BK_COLOUR, TXT_ORIGINAL_COLOUR);

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
    } while(values);    // since we put value(s), we'll search new ones

    return found;
}

// resolve() : Find a solution for the current grid
//
//  mDuration : points to an int that will receive duration
//              of solving process in ms. Can be NULL
//
//  @return : true if a solution was found
//
bool sudoku::resolve(int* mDuration){
    clock_t start(0);
    uint8_t candidate(0);
    position pos(0, true);
    uint8_t status = _findFirstEmptyPos(pos);      // Start from the first empty place

    if (mDuration){
        (*mDuration) = 0;
        start = clock();
    }

    // All the elements "before" the current position - pos -
    // are set with possible/allowed values
    // we'll try to put the "candidate" value
    // (ie. the smallest possible value) at the current position
    while (POS_VALID == status){
        candidate++;   // Next possible value

        if (candidate > VALUE_MAX){
            // No possible value found at this position
            // we'll have to go backward, to the last value setted
            // when no position can be found (ie. all possibles values
            // have  been previously tested), the next pos has an
            // invalid index, -1, and status = POS_INDEX_ERROR
            // no soluton can be found
            if (POS_VALID == (status = _previousPos(pos))){
                // next candidate value is the currently used value + 1
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
                // if the grid is completed, the next pos is
                // out of range ! (status = POS_END_OF_LIST)
                status = _findFirstEmptyPos(pos);

                // At the next pos., we'll use (again) the lowest possible value
                candidate = 0;
            }
        }
    } // while (!status)

    if (POS_END_OF_LIST == status){
        if (mDuration){
            (*mDuration) = ((clock() - start) * 1000 / CLOCKS_PER_SEC);
        }
        return true;    // Found a solution
    }

    // No solution
    return false;
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
    return (_checkLine(pos, value) &&
			_checkRow(pos, value) &&
			_checkTinySquare(pos, value));
}

 // _checkAndSet() : Try to  put the value at the current position
//
//  @pos : position
//  @value : value to put
//
//  @return : true if value is set
//
bool sudoku::_checkAndSet(position& pos, uint8_t value){
    if (_checkLine(pos, value) && _checkRow(pos, value) &&
        _checkTinySquare(pos, value)){
        // set
        elements_[pos.index()].setValue(value, STATUS_ORIGINAL, true);
        return true;
    }

    return false;
}

//
//   Drawings
//

#ifdef DEST_CASIO_CALC
// _drawBackground() : Draw background and the grid's borders
//
void sudoku::_drawBackground(){
    // Erase background
    drect(0, 0, screen_.w - 1, screen_.h - 1, BK_COLOUR);

    // Draw thin borders
    uint16_t posX, posY;
    uint8_t id;
    for (id = 1; id < LINE_COUNT; id++){
        posX = screen_.x + id * SQUARE_SIZE;
        posY = screen_.y + id * SQUARE_SIZE;
        dline(posX, screen_.y, posX, screen_.y + GRID_SIZE, BORDER_COLOUR);   // vert
        dline (screen_.x, posY, screen_.x + GRID_SIZE, posY, BORDER_COLOUR);  // horz
    }

    // and large ext. borders
    uint16_t lSquare(SQUARE_SIZE * 3), thick(BORDER_THICK - 1);
    for (id = 0; id <= 3; id++){
        posX = screen_.x + id * lSquare;
        posY = screen_.y + id * lSquare;
        drect(posX, screen_.y, posX + thick, screen_.y + GRID_SIZE, BORDER_COLOUR);   // vert
        drect(screen_.x, posY, screen_.x + GRID_SIZE, posY + thick, BORDER_COLOUR);   // horz
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
                _drawSingleElement(row, line,
                    pElement->value(), BK_COLOUR,
                    (pElement->isOriginal()?TXT_ORIGINAL_COLOUR:
                    (pElement->isObvious()?TXT_OBVIOUS_COLOUR:TXT_COLOUR)));
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
    uint16_t x(screen_.x + row * SQUARE_SIZE + BORDER_THICK);
    uint16_t y(screen_.y + line * SQUARE_SIZE + BORDER_THICK);

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
        uint16_t dx(1 + (INT_SQUARE_SIZE - w) / 2);
        uint16_t dy(1 + (INT_SQUARE_SIZE - h) / 2);
        dtext_opt(x + dx, y + dy, txtColour, bkColour, DTEXT_LEFT, DTEXT_TOP, sVal, 1);
    }
}
#endif // #ifdef DEST_CASIO_CALC

//
//   Search for obvious values
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

    // No for both of them or yes for both
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

    // No for both of them or yes for both
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
//  If position index is -1, the method will return POS_INDEX_ERROR
//  ie. no solution for this grid
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

#ifdef DEST_CASIO_CALC
// __callbackTick() : Call back function for timer
// This function is used during edition to make selected item blink
//
//  @pTick : pointer to blinking state indicator
//
//  @return : TIMER_CONTINUE if valid
//
int sudoku::__callbackTick(volatile int *pTick){
    *pTick = 1;
    return TIMER_CONTINUE;
}
#endif // #ifdef DEST_CASIO_CALC

// EOF
