//----------------------------------------------------------------------
//--
//--    sudoku.cpp
//--
//--        Implementation of sudoku object - Edition and resolution
//--        of a sudoku grid
//--
//----------------------------------------------------------------------

#include "sudoku.h"
#include "shared/bFile.h"
#include "shared/window.h"
#include "shared/keyboard.h"

#include "sudokuShuffler.h"

#include <time.h>
#include <math.h>

#ifdef DEST_CASIO_CALC
#include <gint/timer.h>
#include <gint/clock.h>

extern bopti_image_t g_pause;

#else
using namespace std;
#endif // #ifdef DEST_CASIO_CALC

// Constructions
//
sudoku::sudoku(){
    // Initializes tinySquares
    uint8_t index;
    for (index = 0; index < TINY_COUNT; index++){
        tSquares_[index].setIndex(index);
    }

    soluce_ = NULL;     // No soluce
    empty();            // Start with an empty grid

    // No hypothese
    for (index = 0; index < HYP_COUNT; index++){
        hypotheses_[index] = {0, HYP_NO_COLOUR, -1};
    }
    hypID_ = -1;

    // No help (yet)
    helpClues_ = MAX_HELP_CLUES;
}

// Copy constructor
//
sudoku::sudoku(sudoku& original)
:sudoku(){
    setElements(original.elements_);    // copy the grid
}

// setElments() : set elements of the grid
//
//  @elements : list of elements
//
void sudoku::setElements(element* elements){
    for (uint8_t index(INDEX_MIN); index <= INDEX_MAX; index++){
        elements_[index] = elements[index];
    }
}

// setScreenRect() : Screen dimensions
//
//  rect : pointer to rect containing new dimensions
//
void sudoku::setScreenRect(const RECT* rect){
    if (rect){
        memcpy(&screen_, rect, sizeof(RECT));
    }
    else{
        screen_ = {0, 0, CASIO_WIDTH, CASIO_HEIGHT};
    }

    // Center the grid vertically
    //screen_.x = (screen_.w - GRID_SIZE) / 2;
    screen_.x = GRID_HORZ_OFFSET;
    screen_.y = (screen_.h - GRID_SIZE) / 2;
}

// display() : Display the grid and it's content
//
//  @update : update screen ?
//
void sudoku::display(bool update){
#ifdef DEST_CASIO_CALC
    _drawBackground();
    _drawContent();
    displayFileName();
    _drawHypotheses();
    if (update){
        dupdate();
    }
#else
    position pos(0, false);
    element* pElement(NULL);
    char car;
    for (uint8_t line(0); line < LINE_COUNT; line++){
        cout << "\t";

        for (uint8_t row(0); row < ROW_COUNT; row++){
            pElement = &elements_[pos];
            car = '0';
            if (!pElement->isEmpty()){
                car += pElement->value();
            }

            cout << car << " - ";

            // Colour ?
            if (pElement->hypColour() != HYP_NO_COLOUR){
                car = (char)(pElement->hypColour());
                cout << car;
            }
            else{
                cout << " ";
            }

            if (row == (ROW_COUNT-1)){
                cout << endl;
            }
            else{
                cout << ',';
            }

            pos++;  // next position
        }
    }

    cout << endl;
#endif // #ifdef DEST_CASIO_CALC
}

// displayFileName() : display current filename
//
void sudoku::displayFileName(){
    if (sFileName_[0]){
#ifdef DEST_CASIO_CALC
        dprint(FILE_TEXT_X, FILE_TEXT_Y, C_BLACK, FILE_TEXT, sFileName_);
#endif // #ifdef DEST_CASIO_CALC
    }
}

// empty() : Empties the grid
//
void sudoku::empty(){
    _freeSoluce();
    for (uint8_t index(INDEX_MIN); index <= INDEX_MAX ; index++){
        elements_[index].empty();
    }

    emptyFileName();
}

// pause() : Show pause screen
//
void sudoku::pause(){
#ifdef DEST_CASIO_CALC
    // Top of image
    dsubimage(0, 0, &g_pause,
            0, 0, IMG_PAUSE_W, IMG_PAUSE_COPY_Y, DIMAGE_NOCLIP);

    // "middle"
    uint16_t y;
    for (y = IMG_PAUSE_COPY_Y;
        y < (IMG_PAUSE_COPY_Y + IMG_PAUSE_LINES); y++){
        dsubimage(0, y, &g_pause,
            0, IMG_PAUSE_COPY_Y,
            IMG_PAUSE_W, 1, DIMAGE_NOCLIP);
    }

    // bottom
    y = CASIO_HEIGHT - IMG_PAUSE_H + IMG_PAUSE_COPY_Y - 1;
    dsubimage(0, y, &g_pause,
            0, IMG_PAUSE_COPY_Y + 1,
            IMG_PAUSE_W, IMG_PAUSE_H - IMG_PAUSE_COPY_Y - 1,
            DIMAGE_NOCLIP);

    dupdate();

    uint car(KEY_CODE_NONE);
    keyboard myKeyboard;
    do{
        car = myKeyboard.getKey();
    }while (KEY_CODE_PAUSE != car && KEY_CODE_EXIT != car);

    if (KEY_CODE_EXIT == car){
        // Close app.
        gint_osmenu();
    }
#endif // #ifdef DEST_CASIO_CALC
}

// create() : Create a new sudoku
//
//  @complexity : Complexity level in {}
//
//  @return : # of clues (ie of non empty elements) or -1 on error
//
int sudoku::create(uint8_t complexity){
    if (complexity < INDEX_MAX){
        uint8_t clues(0);
        while ((clues = _create(complexity)) > COMPLEXITY_EASY){}

#ifndef DEST_CASIO_CALC
        cout << endl << "Clues : " << (int)clues << endl;
#endif // #ifndef DEST_CASIO_CALC

        return clues;
    }

    return -1;
}

// load() : Load a new grid
//
//  @fName : file to load
//
//  @return : 0 on success or an error code
//
uint8_t sudoku::load(const FONTCHARACTER fName){
    emptyFileName();
    if (!fName || !fName[0]){
        return FILE_NO_FILENAME;
    }

    // Load the grid inn a memory buffer
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
        pos++;
    }

    // The new grid is valid
    _newFileName(fName);
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
    emptyFileName();
    if (!fName || !fName[0]){
        return FILE_NO_FILENAME;   // No valid file name
    }

    // Transfer content in a buffer
    char buffer[FILE_SIZE];
    int index(0);
    for (uint8_t lId(0); lId < LINE_COUNT; lId++){
        for (uint8_t cId(0); cId < ROW_COUNT; cId++){
            // '0' means empty !
            buffer[2*index] = (elements_[index].isOriginal()?
                        ('0' + elements_[index].value()):'0');
            buffer[2*index+1] = VALUE_SEPARATOR;
            index++;
        }

        // Replace separator by LF
        buffer[2*index-1] = '\n';
    }

    //buffer[FILE_SIZE] = '\0';   // for trace purpose

    // Save the file
    bFile oFile;
    int fSize(FILE_SIZE);
    oFile.remove(fName);    // Remove the file (if already exist)

    if (!oFile.createEx(fName, BFile_File, &fSize, BFile_WriteOnly)){
        return oFile.getLastError();
    }

    // write the buffer in the file
    bool done(oFile.write(buffer, FILE_SIZE));
    int error(oFile.getLastError());
    oFile.close();

    // Done ?
    if (done){
        _newFileName(fName);
        return FILE_NO_ERROR;
    }

    return error;
}

#ifdef DEST_CASIO_CALC

// edit() : Edit / modify the current grid
//
//  @mode : Edition mode,
//          can be EDIT_MODE_CREATION or EDIT_MODE_MANUAL
//
//  @return : true if grid has been modified or false if left unchanged
//
bool sudoku::edit(uint8_t mode){

    if (EDIT_MODE_CREATION != mode && EDIT_MODE_MANUAL != mode){
        return false;   // Invalid edition mode
    }

    bool modified(false);
    bool cont(true);
    bool showSelected(true);
    bool reDraw(false);
    int eStatus;
    int8_t index;

    position currentPos(0, false);
    position prevPos(0, false);

    if (EDIT_MODE_CREATION == mode){
        revert();   // Remove obvious and found values (if any)
        display();
    }

    menuBar menu;
    MENUACTION action;
    _createEditMenu(menu, mode);
    menu.update();

    // Show selected item in grid
    _drawSingleElement(currentPos,
                    SEL_BK_COLOUR,
                    _elementTxtColour(currentPos, mode, false));
    dupdate();

    // # Values in the grid
    uint8_t values(0), oValues(0);
    for (index = INDEX_MIN; index<=INDEX_MAX ; index++){
        if (!elements_[index].isEmpty()){
            values++;
        }
    }
    oValues = values;

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
            if (!(--tickCount)){
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

        //
        // Change the current value
        //
        case KEY_CODE_0:
            // Remove the value
            if (elements_[currentPos].empty()){
                values--; // one less element
                 modified = true;

                 // First colored element ?
                 if (hypID_ && hypotheses_[hypID_].firstPos == currentPos){
                    hypotheses_[hypID_].firstPos = -1;
                 }
            }

            break;

        case KEY_CODE_1:
            if ((eStatus = _checkAndSet(currentPos, 1, mode)) >= 0){
                modified = true;
                values+=eStatus;
            }
            break;

        case KEY_CODE_2:
            if ((eStatus = _checkAndSet(currentPos, 2, mode)) >= 0){
                modified = true;
                values+=eStatus;
            }
            break;

        case KEY_CODE_3:
            if ((eStatus = _checkAndSet(currentPos, 3, mode)) >= 0){
                modified = true;
                values+=eStatus;
            }
            break;

        case KEY_CODE_4:
            if ((eStatus = _checkAndSet(currentPos, 4, mode)) >= 0){
                modified = true;
                values+=eStatus;
            }
            break;

        case KEY_CODE_5:
            if ((eStatus = _checkAndSet(currentPos, 5, mode)) >= 0){
                modified = true;
                values+=eStatus;
            }
            break;

        case KEY_CODE_6:
            if ((eStatus = _checkAndSet(currentPos, 6, mode)) >= 0){
                modified = true;
                values+=eStatus;
            }
            break;

        case KEY_CODE_7:
            if ((eStatus = _checkAndSet(currentPos, 7, mode)) >= 0){
                modified = true;
                values+=eStatus;
            }
            break;

        case KEY_CODE_8:
            if ((eStatus = _checkAndSet(currentPos, 8, mode)) >= 0){
                modified = true;
                values+=eStatus;
            }
            break;

        case KEY_CODE_9:
            if ((eStatus = _checkAndSet(currentPos, 9, mode)) >= 0){
                modified = true;
                values+=eStatus;
            }
            break;

        // Pause
        case KEY_CODE_PAUSE:
            pause();
            display();
            menu.update();
            break;

        //
        // Hyptoheses & manual solving
        //
        case IDM_MANUAL_ACCEPT:
            if (_onManualAccept()){
                display();
            }
            break;

        case IDM_MANUAL_HELP:
            if (-1 != (index = _onManualHelp(menu))){
                // Select added-element
                currentPos = index;
            }
            break;

        case IDM_MANUAL_REJECT:
        {
            uint8_t count;
            if ((count = _onManualReject())){
                values-=count;

                if (-1 != hypotheses_[hypID_].firstPos){
                    // Move to  element at 'first' pos.
                    currentPos = hypotheses_[hypID_].firstPos;
                }
                hypotheses_[hypID_].firstPos = -1; // No longer needed

                // At least one element's colour has changed => redraw
                display();
            }
            break;
        }

        // Cancel modifications
        case IDM_EDIT_CANCEL:
            revert();
            cont = modified = false;
            break;

        // Check grid
        //
        case IDM_EDIT_CHECK:
            _onEditCheckSudoku();
            break;

        // Exit from "edit" mode
        case KEY_MENU:
        //case KEY_EXIT:
        case IDM_EDIT_OK:
            cont = false;
            break;

        // Other messages
        default:
            if (action.value >= IDM_MANUAL_COLOUR_FIRST &&
                action.value < (IDM_MANUAL_COLOUR_FIRST + HYP_COUNT)){
                // User choose a checkbox
                _onHypChanged(menu, action.value,
                        menuBar::isBitSet(action.state, ITEM_STATE_CHECKED));
            }

            break;
        } // switch (action.value)

        if (reDraw || prevPos != currentPos){
            // if sel. changed, erase previously selected element
            if (prevPos != currentPos){
                _drawSingleElement(prevPos,
                    (prevPos.squareID()%2)?GRID_BK_COLOUR_DARK:GRID_BK_COLOUR,
                    _elementTxtColour(prevPos, mode, false));
            }

            if (showSelected){
                // Hilight the new value (or have it blink)
                _drawSingleElement(currentPos,
                    SEL_BK_COLOUR,
                    _elementTxtColour(currentPos, mode, true));
            }
            else{
                // "unhilite" for blinking effect
                _drawSingleElement(currentPos,
                    (currentPos.squareID()%2)?GRID_BK_COLOUR_DARK:GRID_BK_COLOUR,
                    _elementTxtColour(currentPos, mode, false));
            }

            // # values
            if (oValues != values){
                /*
                drect(VALUES_X, VALUES_Y,
                    CASIO_WIDTH - 1,
                    VALUES_Y + TEXT_V_OFFSET - 1,
                    SCREEN_BK_COLOUR);
                */
                dprint_opt(VALUES_X, VALUES_Y,
                    C_BLACK, SCREEN_BK_COLOUR,
                    DTEXT_LEFT, DTEXT_TOP,
                    VALUES_TEXT, values, ROW_COUNT * LINE_COUNT);

                oValues = values;

                if (values >= VALUES_COUNT){
                    cont = false;   // No more value to find
                }
            }

            dupdate();
            prevPos = currentPos;
            reDraw = false;
        }
    } // while (cont)

    if (timerID >= 0){
        timer_stop(timerID);    // stop the timer
    }

    // Completed ?
    if (values >= VALUES_COUNT){
        window popup;
        window::winInfo wInf;
        wInf.style = WIN_STYLE_DBORDER | WIN_STYLE_HCENTER;
        wInf.pos.y = WIN_SOL_Y;
        wInf.pos.w = WIN_SOL_W;
        wInf.pos.h = WIN_SOL_H;
        wInf.bkColour = COLOUR_LT_GREY;
        popup.create(wInf);

        popup.drawText(WIN_FOUND_TXT);
        popup.update();

        getkey();   // Wait for any key to be pressed

        popup.close();
        display();
    }

    // Remove all coloured hyp.
    if (EDIT_MODE_MANUAL == mode){
        for (uint8_t index(INDEX_MIN); index<=INDEX_MAX ; index++){
            elements_[index].setHypColour(HYP_NO_COLOUR);
        }

        // Redraw the whole grid
        display();
    }
    else{
        // Draw in valid state (or erase)
        _drawSingleElement(currentPos,
                (currentPos.squareID()%2)?GRID_BK_COLOUR_DARK:GRID_BK_COLOUR,
                _elementTxtColour(currentPos, mode, false));
    }

    return modified;
}

#endif // #ifdef DEST_CASIO_CALC

// findObviousValues() : Find all the obvious values
//
//  @return : #obvious values found
//
uint8_t sudoku::findObviousValues(){
    uint8_t found(0), values(0);
    do{
        values = _findObviousValues();
        found += values;
    } while(values);    // since we found value(s), we'll search new ones

    return found;
}

// resolve() : Find a solution for the current grid
//
//  The algorithm will go forward to seek value and backward each
//  time a value can't be found at a given pos.
//  When going backward, if position reaches sPos (if not NULL)
//  or exits the grid, the method ends with no solution.
//
//  @mDuration : points to an int that will receive duration
//              of solving process in ms. Can be NULL
//  @soluce : Pointer to a table to copy the solution into. If NULL
//              or if no soluce is found no copy is done
//
//  @return : true if a solution was found
//
bool sudoku::resolve(int* mDuration, int8_t** soluce){
    clock_t start(0);

    if (mDuration){
        (*mDuration) = 0;
        start = clock();
    }

#ifdef DEST_CASIO_CALC
    window waitWindow;
    window::winInfo wInf;
    wInf.style = WIN_STYLE_DEFAULT;
    wInf.pos.y = WIN_SOL_Y;
    wInf.pos.w = WIN_SOL_W;
    wInf.pos.h = WIN_SOL_H;
    wInf.bkColour = COLOUR_LT_GREY;
    waitWindow.create(wInf);
    waitWindow.drawText(WIN_SEARCH_TXT);
    waitWindow.update();
#endif // #ifdef DEST_CASIO_CALC

    bool found(_resolve(INDEX_MIN)); // Try to find the first solution

    // Copy duration
    if (mDuration){
        (*mDuration) = ((clock() - start) * 1000 / CLOCKS_PER_SEC);
    }

    // Copy soluce ?
    if (found && soluce){
        (*soluce) = (int8_t*)malloc(sizeof(int8_t) * LINE_COUNT * ROW_COUNT);
        if ((*soluce)){
            _copyElements(*soluce);
        }
    }

#ifdef DEST_CASIO_CALC
    waitWindow.close(); // Close the window
#endif // #ifdef DEST_CASIO_CALC

    return found;
}

// findNextStartPos() : Find the next "starting" postion
//
//  This enables to search for another solution or check wether
//  a found solution is unique or not
//
//  @start : Position to search from
//          @start will point to the new position has been found
//  @startVal : New starting value
//
//  @return : true if a valid position has been found
//
bool sudoku::findNextStartPos(position &start, int8_t& startVal){
    int8_t elements[INDEX_MAX+1];
    _copyElements(elements);       // Keep a copy of current completed grid

    bool found(false);
    startVal = -1; // Error

    if (POS_VALID == start.status()){
        position next(start);
        int8_t oValue(elements_[next].value());
        int8_t value(oValue + 1);   // Next value

        _revertFrom(next);

        while (!found && next < INDEX_MAX){
            if (value <= VALUE_MAX){
                if (false == (found = _checkValue(next, value))){
                    // Try next value
                    value++;
                }
            }
            else{
                // Put back previous val.
                elements_[next].setValue(oValue);

                // Try next pos (ie. next "set" value)
                while (++next < INDEX_MAX){
                    if (elements[next] < 0){
                        value = -1 * elements[next] + 1;
                    }
                }
            }
        }

        if (found){
            start = next;    // (new or not) stating pos.
            startVal = value;   // New starting value
        }
    }

    return found;   // ?
}

// multipleSolutions() : Check wether a grid has one or many solutions
//
//  This method doesn't seek for all possible solutions since it stops
//  when no soluce is found or at the second one.
//
//  @return : 0 if the grid has no solution, 1 if a unique solution has
//            been found -1 if many solutions may be founded (2 at least).
//
int sudoku::multipleSolutions(){
    int count(0);
    position start(INDEX_MIN), valid(INDEX_MIN);
    int8_t newVal(0);
    bool finished(false);

    while (!finished && _resolve(&start)){
        if (count++){
            // Found 2 solutions => stop searchning
            return -1;
        }

        // Search next element to change
        while (elements_[valid].isOriginal()){
            valid++;
        }
        start = valid;

        sudoku next(*this);
        if (next.findNextStartPos(start, newVal)){
            _revertFrom(start);   // remove previously founded vals
            elements_[start].setValue(newVal, STATUS_SET);
            valid = start;
        }
        else{
            // No more starting pos.
            finished = true;
        }
    }

    return count;
}

//
// Internal methods
//

// create() : Create a new sudoku
//
//  @complexity : Complexity level in {}
//
//  @return : # of clues (ie of non empty elements)
//
uint8_t sudoku::_create(uint8_t complexity){
    // step 1 : start from a complete grid
    empty();
    resolve();

    // step 2 : shuffles elements
    sudokuShuffler shuffler(elements_);
    shuffler.shuffleValues();

    // step 3 : rearrange columns
    shuffler.shuffleColumns();

    // step 4: rearrange rows
    shuffler.shuffleRows();

    // step 5 : rearrange block of columns
    shuffler.shuffleColumnBlocks();

    // step 6 : rearrange block of lines
    shuffler.shuffleRowBlocks();

    // step 7 : all elements are "original"
    for (uint8_t index(INDEX_MIN); index <= INDEX_MAX; index++){
        elements_[index].setStatus(STATUS_ORIGINAL | STATUS_SET);
    }

#ifndef DEST_CASIO_CALC
    display();
    cout << " <<<<<<< ";
#endif // #ifndef DEST_CASIO_CALC

    // step 8 : remove values according to expected complexity
    uint8_t maxIndex = ROW_COUNT * LINE_COUNT;
    uint8_t clues(maxIndex);  // Starting with full grid
    bool stop(false);
    uint8_t val, blocked(true);
    uint8_t index;
    sudoku tester;

    while (!stop && clues > complexity){
        //index.moveTo(rand() % LINE_COUNT, rand() % ROW_COUNT);
        index = (rand() % maxIndex);

        if (!(elements_[index] == 0)){
            val = elements_[index].empty();

            // Still a unique sol ?
            tester.setElements(elements_);
            if (1 == tester.multipleSolutions()){
                // Yes => continue
                elements_[index].empty();
                clues--;
            }
            else{
                // return to previous stats
                elements_[index].setValue(val, true);

                // Try to many times => accept this solution
                if (++blocked > COMPLEXITY_BLOCKED_MAX){
                    stop = true;
                }
            }
        }
    }

    return clues;
}

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

// _checkAndSet() : Try to put the value at the given position
//
//  @pos : position
//  @value : value to put
//  @mode : edition mode
//
//  @return : -1 if can't be changed, 0 if changed an existing value,
//          1 if new value set
//
int sudoku::_checkAndSet(position& pos, uint8_t value, uint8_t mode){
    uint8_t status(elements_[pos].status());
    bool editGrid(true);
    if (EDIT_MODE_MANUAL == mode){
        // Check wether element's value can be changed
        if (STATUS_ORIGINAL == status){
            return -1;
        }

        editGrid = false;   // manual mode
    }

    uint8_t oValue(elements_[pos].value());  // current val

    // Check value at the given pos.
    //  New allowed value or same value with different hyp. colour
    int hypColour(_hypColour(hypID_));
    if ((oValue == value && elements_[pos.index()].hypColour() != hypColour)
        ||
        (_checkLine(pos, value) && _checkRow(pos, value) &&
        _checkTinySquare(pos, value))){
        // Set new value and colour
        elements_[pos.index()].setValue(value, editGrid);
        elements_[pos.index()].setHypColour(hypColour);

        // First value with this hyp ?
        if (-1 == hypotheses_[hypID_].firstPos){
            hypotheses_[hypID_].firstPos = pos.index();
        }

        return (oValue?0:1);
    }

    return -1;  // Not set
}

// _onEditCheckSudoku() : Check wether grid can be solved
//
void sudoku::_onEditCheckSudoku(){
    sudoku tester(*this);
    int count = tester.multipleSolutions(); // try to find solution(s)

#ifdef DEST_CASIO_CALC
    window output;
    window::winInfo wInf;
    wInf.title = (char*)WIN_SOL_TITLE;
    wInf.style = WIN_STYLE_DBORDER | WIN_STYLE_HCENTER;
    wInf.pos.y = WIN_SOL_Y;
    wInf.pos.w = WIN_SOL_W;
    wInf.pos.h = WIN_SOL_H;
    wInf.bkColour = COLOUR_LT_GREY;
    output.create(wInf);

    switch (count){
        case -1:
            output.drawText(STR_MULTIPLE_SOL, -1, -1, COLOUR_RED);
            break;

        case 1:
            output.drawText(STR_ONE_SOL);
            break;

        case 0:
        default:
            output.drawText(STR_NO_SOL, -1, -1, COLOUR_RED);
            break;
    }

    output.update();

    // Wait for any key to be pressed
    getkey();
    output.close();

    display();
#else
    cout << "Check : " << (int)count << endl;
#endif // #ifdef DEST_CASIO_CALC
}

//
//   Drawings
//

#ifdef DEST_CASIO_CALC
// _drawBackground() : Draw background and the grid's borders
//
void sudoku::_drawBackground(){
    // Erase background
    drect(0, 0, screen_.w - 1, screen_.h - 1, SCREEN_BK_COLOUR);

    // Draw odd tiny rect.
    uint16_t posX(screen_.x), posY(screen_.y), index(0);
    uint8_t tSize(3*SQUARE_SIZE);
    for (uint8_t line(0); line < 3; line++){
        for (uint8_t co(0); co < 3; co++){
            drect(posX, posY,
                posX + tSize - 1, posY + tSize - 1,
                ((index++)%2)?GRID_BK_COLOUR_DARK:GRID_BK_COLOUR);
            posX += tSize;
        }

        posX = screen_.x;
        posY += tSize;
    }

    // Draw thin (internal) borders
    uint8_t id;
    for (id = 1; id < LINE_COUNT; id++){
        posX = screen_.x + id * SQUARE_SIZE;
        posY = screen_.y + id * SQUARE_SIZE;
        dline(posX, screen_.y,
            posX, screen_.y + GRID_SIZE, INT_BORDER_COLOUR);   // vert
        dline(screen_.x, posY,
            screen_.x + GRID_SIZE, posY, INT_BORDER_COLOUR);  // horz
    }

    // Draw large (external) borders
    uint16_t lSquare(SQUARE_SIZE * 3), thick(BORDER_THICK - 1);
    for (id = 0; id <= 3; id++){
        posX = screen_.x + id * lSquare;
        posY = screen_.y + id * lSquare;
        drect(posX, screen_.y,
            posX + thick, screen_.y + GRID_SIZE,
            EXT_BORDER_COLOUR);   // vert
        drect(screen_.x, posY,
            screen_.x + GRID_SIZE, posY + thick,
            EXT_BORDER_COLOUR);   // horz
    }
}

// _drawContent() : Draw all the elements
//
void sudoku::_drawContent(){
    position pos(0, false);
    element* pElement(NULL);
    uint8_t line, row;
    for (line = 0; line < LINE_COUNT; line++){
        for (row = 0; row < ROW_COUNT; row++){
            pElement = &elements_[pos];
            if (!pElement->isEmpty()){
                _drawSingleElement(pos,
                    (pos.squareID()%2)?GRID_BK_COLOUR_DARK:GRID_BK_COLOUR,
                    (pElement->isOriginal()?TXT_ORIGINAL_COLOUR:
                    (pElement->isObvious()?TXT_OBVIOUS_COLOUR:TXT_COLOUR))
                    );
            }
            pos++; // next element
        }
    }
}
#endif // #ifdef DEST_CASIO_CALC

// _drawSingleElement : draw a single element of the grid
//
//  @pos: Position of the element
//  @bkColour : background colour
//  @txtColour : text colour
//
void sudoku::_drawSingleElement(position pos, int bkColour, int txtColour){
#ifdef DEST_CASIO_CALC
    element* pElement(&elements_[pos]);
    uint8_t row(pos.row()), line(pos.line()), value(pElement->value());
    int hypColour(pElement->hypColour());
    uint16_t x(screen_.x + row * SQUARE_SIZE + BORDER_THICK);
    uint16_t y(screen_.y + line * SQUARE_SIZE + BORDER_THICK);

    // Erase background
    drect(x, y, x + INT_SQUARE_SIZE, y + INT_SQUARE_SIZE, bkColour);

    // display the value (if valid)
    if (value){
        // Hypothese colour
        if (HYP_NO_COLOUR != hypColour){
            drect(x + INT_SQUARE_SIZE - HYP_SQUARE_SIZE - 1,
                y + 1,
                x + INT_SQUARE_SIZE - 1,
                y + HYP_SQUARE_SIZE,
                hypColour);
        }

        char sVal[2];
        sVal[0] = '0' + value;
        sVal[1] = '\0';

        // text dims
        int w, h;
        dsize(sVal, NULL, &w, &h);

        // Center the text
        uint16_t dx(1 + (INT_SQUARE_SIZE - w) / 2);
        uint16_t dy(1 + (INT_SQUARE_SIZE - h) / 2);
        dtext_opt(x + dx, y + dy,
            txtColour, NO_COLOR, DTEXT_LEFT, DTEXT_TOP, sVal, 1);
    }
#endif // #ifdef DEST_CASIO_CALC
}

// _drawHypotheses() : Draw hypotheses list
//
void sudoku::_drawHypotheses(){
#ifdef DEST_CASIO_CALC

    // Erase previous list
    drect(HYP_LIST_X, HYP_LIST_Y,
            HYP_LIST_X + HYP_LIST_W + HYP_COUNT * HYP_LIST_OFFSET,
            HYP_LIST_Y + HYP_LIST_H + HYP_COUNT * HYP_LIST_OFFSET,
            SCREEN_BK_COLOUR);

    // Draw hypotheses' stack
    if (hypID_ >= 0){
        dtext(HYP_LIST_TEXT_X, HYP_LIST_TEXT_Y, C_BLACK, HYP_LIST_TEXT);

        int x(HYP_LIST_X), y(HYP_LIST_Y);
        for (uint8_t index(0); index <= hypID_; index++){
            drect(x, y, x + HYP_LIST_W, y + HYP_LIST_H,
                    hypotheses_[index].colour);
            x+=HYP_LIST_OFFSET;
            y+=HYP_LIST_OFFSET;
        }
    }
#endif // #ifdef DEST_CASIO_CALC
}

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
        pos++;

    }   // for

    return found;
}

// _checkObviousValue() : Is there an obvious value for the given pos ?
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
    if ((-1 == firstPos.line && -1 == secondPos.line)
            || (-1 != firstPos.line && -1 != secondPos.line)){
        return 0;
    }

    // Just one square misses the value => we'll try to put
    // this value in the correct line
    //
    uint8_t candidate, candidateLine;
    if (-1 == firstPos.line){
        candidate = firstID;
        candidateLine = 2 * (tSquares_[firstID].topLine() + 1)
                        - secondPos.line - pos.line() + 1;
    }
    else{
        candidate = secondID;
        candidateLine = 2 * (tSquares_[secondID].topLine() + 1)
                        - firstPos.line - pos.line() + 1;
    }

    // Try to put the value in the line
    //
    bool found(false);
    position candidatePos(0, true), foundPos;
    candidatePos.moveTo(candidateLine,
                        tSquares_[candidate].topRow());   // First row ID

    for (uint8_t row = 0; row < TINY_ROW_COUNT; row++){
        if (elements_[candidatePos].isEmpty() &&
            _checkValue(candidatePos, value)){
            // found a valid pos. in the line for the value
            if (found){
                // Already a candiate
                return 0;
            }

            foundPos.set(candidatePos);
            found = true;
        }

        // Next row
        candidatePos++;
    }

    // Did we find a position ?
    if (found){
        // Yes => one more obvious val.
        elements_[foundPos].setValue(value, STATUS_OBVIOUS);
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
    if ((-1 == firstPos.row && -1 == secondPos.row)
        || (-1 != firstPos.row && -1 != secondPos.row)){
        return 0;
    }

    // Just one square misses the value
    // => we'll try to put this value in the correct line
    //
    uint8_t candidate, candidateRow;
    if (firstPos.row == -1){
        candidate = firstID;
        candidateRow = 2 * (tSquares_[firstID].topRow() + 1)
                    - secondPos.row - pos.row() + 1;
    }
    else{
        candidate = secondID;
        candidateRow = 2 * (tSquares_[secondID].topRow() + 1)
                    - firstPos.row - pos.row() + 1;
    }

    // Try to put the value ...
    //
    bool found(false);
    position candidatePos(0, true), foundPos;
    candidatePos.moveTo(tSquares_[candidate].topLine(), candidateRow);

    for (uint8_t line = 0; line < TINY_LINE_COUNT; line++){
            if  (elements_[candidatePos].isEmpty()
                    && _checkValue(candidatePos, value)){
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

// _resolve() : Find a solution for the current grid
//
//  The algorithm will go forward to seek value and backward each
//  time a value can't be found at a given pos.
//  We going backward, if position reaches sPos (if not NULL)
//  or exit the grid, the method end with no solution.
//
//  @sPos : Start position. If NULL the method will start
//          from the first empty pos found in the grid
//
//  @return : true if a solution was found
//
bool sudoku::_resolve(position* sPos){
    uint8_t candidate;
    position pos(0, true);
    uint8_t startIndex;
    uint8_t status;

    if (NULL == sPos){
      status = _findFirstEmptyPos(pos);  // Start from beginning
      startIndex = 0;
      candidate = 0;
    }
    else{
        pos = *sPos;    // Use given pos as start index
        startIndex = pos;
        status = pos.status();
        candidate = elements_[pos].value() - 1; // ++ in the loop !
        elements_[pos].setValue(0);
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
                if (pos >= startIndex){
                    // next candidate value is the currently used value + 1
                    candidate = elements_[pos].empty();
                }
                else{
                    // return to start pos => no soluce
                    status = POS_INDEX_ERROR;
                }
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

                // At the next pos.,
                // we'll use (again) the lowest possible value
                candidate = 0;
            }
        }
    } // while (!status)

    // No solution
    return (POS_END_OF_LIST == status);
}

// _onManualHelp() : Help the user to solve the current grid
//
//  A new clue element is shown
//
//  @menu : Edit sub-menu
//
//  @return : index of added element or -1 if none were added
//
int8_t sudoku::_onManualHelp(menuBar& menu){
    if (!helpClues_){
        // Already helped, ... too many times
        return -1;
    }

    // No sol. in memory ?
    if (NULL == soluce_){
        srand((unsigned int)clock());   // Set root

        sudoku solver(*this);
        if (!solver.resolve(NULL, &soluce_)){
            // Unable to find a solution (???)
            return -1;
        }
    }

    // # of free items
    uint8_t freeItems(ROW_COUNT * LINE_COUNT);
    int8_t index;
    for (index = INDEX_MIN; index < INDEX_MIN; index++){
        if (elements_[index].isEmpty()){
            freeItems--;
        }
    }

    if (freeItems <= MIN_CLUE_COUNT){
        return -1; // No need to help the use, the grid is nearly full
    }

    // Randomly select an item in the free ones
    uint8_t clueID(1 + (rand() % freeItems));
    index = -1;
    while (clueID){
        if (elements_[++index].isEmpty()){
            clueID--;
        }
    }

    // Found one !
    int8_t helpValue(soluce_[index] * -1);  // found value is < 0
    elements_[index].setValue(helpValue, true);
    display();

    if (!(--helpClues_)){
        menu.activateItem(IDM_MANUAL_HELP, SEARCH_BY_ID, false);
    }

    return index;
}

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
    current--;
    while (POS_VALID == current.status()
            && !elements_[current].isChangeable()){
        current--;
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

// _createEditMenu() : Create sub-menu displayed on edition mode
//
//  @menu : Menu bar to modifiy
//  @editMode : Edition mode (game or creation)
//
void sudoku::_createEditMenu(menuBar& menu, uint8_t editMode){
    if (EDIT_MODE_CREATION == editMode){
        // Just 3 buttons on edit mode
        menu.appendItem(IDM_EDIT_OK, IDS_EDIT_OK);
        menu.appendItem(IDM_EDIT_CHECK, IDS_EDIT_CHECK);
        menu.addItem(MENU_POS_RIGHT - 1, IDM_EDIT_CANCEL, IDS_EDIT_CANCEL);
    }
    else{
        // Some items should be drawn by this class
        // and not by the menubar object itself
        menu.setMenuDrawingCallBack(_ownMenuItemsDrawings);

        // Coloured hyp. submenu
        //  each colour is stored in the ownerData member ot item struct
        menuBar hypMenu;
        PMENUITEM item(hypMenu.appendCheckbox(IDM_MANUAL_COLOUR_FIRST,
                                NULL, ITEM_STATE_DEFAULT,
                                ITEM_STATUS_OWNERDRAWN));
        item->ownerData = HYP_COLOUR_YELLOW;

        item = hypMenu.appendCheckbox(IDM_MANUAL_COLOUR_FIRST + 1,
                                NULL, ITEM_STATE_DEFAULT,
                                ITEM_STATUS_OWNERDRAWN);
        item->ownerData = HYP_COLOUR_BLUE;

        item = hypMenu.appendCheckbox(IDM_MANUAL_COLOUR_FIRST + 2,
                                NULL, ITEM_STATE_DEFAULT,
                                ITEM_STATUS_OWNERDRAWN);
        item->ownerData = HYP_COLOUR_GREEN;

        item = hypMenu.appendCheckbox(IDM_MANUAL_COLOUR_FIRST + 3,
                                NULL, ITEM_STATE_DEFAULT,
                                ITEM_STATUS_OWNERDRAWN);
        item->ownerData = HYP_COLOUR_RED;

        // Draw coloured checkboxes
        hypMenu.setMenuDrawingCallBack(_ownMenuItemsDrawings);

        item = menu.appendSubMenu(&hypMenu,
                    IDM_MANUAL_HYP_SUBMENU, IDS_MANUAL_HYP_SUBMENU,
                    ITEM_STATE_DEFAULT, ITEM_STATUS_OWNERDRAWN);
        item->ownerData = HYP_NO_COLOUR;

        item = menu.appendItem(IDM_MANUAL_ACCEPT, IDS_MANUAL_ACCEPT,
                    ITEM_STATE_DEFAULT, ITEM_STATUS_OWNERDRAWN);
        item->ownerData = HYP_NO_COLOUR;

        menu.appendItem(IDM_MANUAL_REJECT, IDS_MANUAL_REJECT);
        menu.appendItem(IDM_MANUAL_HELP, IDS_MANUAL_HELP);
        menu.addItem(MENU_POS_RIGHT, IDM_MANUAL_END, IDS_MANUAL_END);
    }
}

//
// Coloured hypotheses & manual solving process
//

// _elementTxtColour() : Get element's text colour for edition
//
//  @pos : Element's position
//  @editMode : Edition mode (game or creation)
//  @selected :Is element in selectedmode ?
//
//  @return : Colour to use
//
int sudoku::_elementTxtColour(position& pos, uint8_t editMode, bool selected){
    if (EDIT_MODE_CREATION == editMode){
        return (TXT_ORIGINAL_COLOUR);   // always use "original" colour
    }

    if (elements_[pos].isObvious()){
        return TXT_OBVIOUS_COLOUR;
    }
    else{
        if (elements_[pos].isOriginal()){
            return TXT_ORIGINAL_COLOUR;
        }
        else{
            return (selected?SEL_TXT_GAME_COLOUR:TXT_COLOUR);
        }
    }
}

//  _ownMenuItemsDrawings() : Draw an "ownerdraw" menu item
//
// @bar : Pointer to the bar containing the item to be drawn
//  @item : Pointer to a MENUITEM strcut containing informations
//          concerning the item to draw
//  @anchor : Position of the item in screen coordinates
//  @style : Drawing style ie. element(s) to draw
//
//  @return : False on error(s)
//
bool sudoku::_ownMenuItemsDrawings(PMENUBAR const bar,
            PMENUITEM const  item, RECT* const anchor, int style){
#ifdef DEST_CASIO_CALC
    if (item){
        int x, y;
        if (menuBar::isBitSet(item->status, ITEM_STATUS_CHECKBOX)){
            // Draw the checkbox
            menuBar::defDrawItem(bar, item, anchor, style);

            // Draw a rectangle  whose colour is store in ownerData member
            x = anchor->x + MENU_IMG_WIDTH + 4; // After the image
            y = anchor->y + (anchor->h - HYP_SQUARE_SIZE) / 2;

            drect(x, y,
                anchor->x + anchor->w -3, y + HYP_SQUARE_SIZE - 1,
                menuBar::isBitSet(item->state, ITEM_STATE_INACTIVE)?
                    GRID_BK_COLOUR:item->ownerData);
        }
        else{
            // Draw background
            menuBar::defDrawItem(bar, item, anchor, MENU_DRAW_BACKGROUND);

            if (item->ownerData != HYP_NO_COLOUR){
                // Draw a rectangle on the right of item
                //x = anchor->x + anchor->w - HYP_SQUARE_SIZE - 3;
                x = anchor->x + 2;
                y = anchor->y + (anchor->h - HYP_SQUARE_SIZE) / 2;

                drect(x, y,
                    //x + HYP_SQUARE_SIZE- 1,
                    anchor->x + anchor->w -3,
                    y + HYP_SQUARE_SIZE - 1,
                    item->ownerData);
            }

            // Then, draw the text and borders
            menuBar::defDrawItem(bar, item, anchor,
                                MENU_DRAW_TEXT | MENU_DRAW_BORDERS);
        }
    }
    else{
        menuBar::defDrawItem(bar, item, anchor, style);
    }
#endif // #ifdef DEST_CASIO_CALC

    return true;
}

// _onManualReject() : Reject all the hypothese's values
//
//  When rejected, all elements with the given hyp. colour
//  will be cleared
//
//  @return : Count of elements concerned
//
uint8_t sudoku::_onManualReject(){
    uint8_t count(0);
    if (hypID_>=0){
        count = _hypReject(hypotheses_[hypID_].colour);
    }

    return count;
}

// _hypAccept() : Accept all the hypothese's values
//
//  All elements with the @colFrom colour
//  will have their colour changed to the @colTo colour
//
//  @colFrom : Original hyp. col.
//  @colTo : Drest. hyp. colour
//
//  @return : Count of elements whose colour have changed
//
uint8_t sudoku::_hypAccept(int colFrom, int colTo){
    uint8_t count(0);
    if (colFrom != colTo){
        for (uint8_t index(INDEX_MIN); index <= INDEX_MAX; index++){
            if (colFrom == elements_[index].hypColour()){
                elements_[index].setHypColour(colTo);
                count++;
            }
        }
    }

    return count;
}

// _hypReject() : Reject all the hypothese's values
//
//  When rejected, all elements with the @colFrom  hyp. colour
//  will be cleared
//
//  @colFrom : Colour of rejected elements
//
//  @return : Count of elements concerned
//
uint8_t sudoku::_hypReject(int colFrom){
    uint8_t count(0);
    if (colFrom != HYP_NO_COLOUR){
        for (uint8_t index(INDEX_MIN); index <= INDEX_MAX; index++){
            if (colFrom == elements_[index].hypColour()){
                elements_[index].empty();
                count++;
            }
        }
    }

    return count;
}

// _hypPush() : Push a new coloured hypthese on top of stack
//
//  @menu : Hyp. colours menu
//  @menuID : ID of menu colour
//  @newColour : new colour
//
//  @return : previous ID (or -1 on error)
//
int sudoku::_hypPush(menuBar& menu, int menuID, int newColour){
    int8_t prev(hypID_);

    // Add to stack
    hypotheses_[++hypID_].colour = newColour;
    hypotheses_[hypID_].menuID = menuID;

    // Update menus
    _hypUpdateMenu(menu, newColour, _hypColour(hypID_-1));

    return hypotheses_[prev].menuID;
}

// _hypPop() : Remove the hypothese from the top of the stack
//
//  @return : previous ID
//
int sudoku::_hypPop(menuBar& menu){
    int prev(-1);
    if (hypID_ >= 0){
        prev = (hypID_?hypotheses_[hypID_-1].menuID:-1);

        // remove from top
        hypotheses_[hypID_--] = {0, HYP_NO_COLOUR, -1};

        // Update menus
        _hypUpdateMenu(menu, _hypColour(hypID_), _hypColour(hypID_-1));
    }

    return prev;
}

// _hypUpdateMenu() : Update menu according to new selected colour
//
//  @menu : menubar
//  @curCol : New 'currrent' colour
//  @prevCol : previous col
//
void sudoku::_hypUpdateMenu(menuBar& menu, int curCol, int prevCol){
    if (curCol == prevCol){
        return;
    }

    // Update menus
    PMENUITEM item(menu.getItem(IDM_MANUAL_HYP_SUBMENU, SEARCH_BY_ID));
    if (item){
        item->ownerData = curCol;   // Current hyp's col.
    }

    if ((item = menu.getItem(IDM_MANUAL_ACCEPT, SEARCH_BY_ID))){
        // Previous hyp's col
        item->ownerData = prevCol;
    }
}

// _onHypChanged() : Change coloured hyp.
//
//  @menu : Hyp. colours menu
//  @newHypID : Menu ID
//  @checked : true if item is checked
//
//  @return : current col.
//
int sudoku::_onHypChanged(menuBar& menu, int newHypID, bool checked){

    if (newHypID == hypID_){
        return hypotheses_[hypID_].colour; // No change
    }

    PMENUITEM item(menu.findItem(newHypID, SEARCH_BY_ID));
    if (item){
        int prev;
        if (checked){
            prev = _hypPush(menu, newHypID, item->ownerData);
        }
        else{
            // Reject values associated with the previous colour
            // ie. accept with the prev. colour
            _hypAccept(_hypColour(hypID_), _hypColour(hypID_-1));
            prev = _hypPop(menu);
        }

        // "previous" item's state
        if (prev >= 0){
            menu.activateItem(prev, SEARCH_BY_ID, !checked);
        }

        // Activate current hyp. col in menu
        menu.selectByIndex(checked?hypotheses_[hypID_].menuID:prev, true);

        // menu.showParentBar(false);   // Return to "manual" menubar
        menu.update();
        display();

        return item->ownerData;
    }

    // error ?
    return HYP_NO_COLOUR;
}

//
// Utilities
//

// _newFileName() : Set current file name
//
//  @fName : New FQN
//
void sudoku::_newFileName(FONTCHARACTER fName){
    if (!FC_ISEMPTY(fName)){
        char sName[BFILE_MAX_PATH + 1];
        bFile::FC_FC2str(fName, sName); // convert to char*
        char* name = strrchr(sName, CHAR_PATH_SEPARATOR);
        strcpy(sFileName_, (name?++name:sName)); // No path ?
    }
    else{
        emptyFileName();
    }
}

// _revertFrom : Return to the original state
//
//  All values "after" @from prosition will be set to 0 if
//  not 'original'
//
//  @from : starting position (included)
//
void sudoku::_revertFrom(uint8_t from){
    for (uint8_t index(from); index <= INDEX_MAX ; index++){
        if (!elements_[index].isOriginal()){
            elements_[index].empty();
        }
    }
}

//  _copyElements() : Create a copy of current elements table
//
//  Elements values will be : > 0 for orignal values, 0 for empty
//  values et < 0 for 'found' values
//
//  @dest : destination table
//
void sudoku::_copyElements(int8_t* dest){
    uint8_t value;
    for (uint8_t index(INDEX_MIN); index <= INDEX_MAX; index++){
        value = elements_[index].value();
        dest[index] = value * (elements_[index].isOriginal()?1:-1);
    }
}

//  _freeSoluce() : Free the memory allocated for a solution
//
void sudoku::_freeSoluce(void){
    if (soluce_){
        free(soluce_);
        soluce_ = NULL;
    }
}

// EOF
