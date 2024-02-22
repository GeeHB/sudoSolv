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
#include "menus.h"

#include "element.h"
#include "position.h"
#include "tinySquare.h"

#include "shared/bFile.h"

// Error codes
//
#define FILE_NO_ERROR           BFILE_NO_ERROR
#define FILE_INVALID_FILENAME   (BFILE_LAST_ERROR_CODE + 1) // File doesn't exist
#define FILE_INVALID_FILESIZE   (BFILE_LAST_ERROR_CODE + 2)
#define FILE_NO_FILENAME        (BFILE_LAST_ERROR_CODE + 3)
#define FILE_IO_ERROR           (BFILE_LAST_ERROR_CODE + 4)
#define FILE_INVALID_LINE       (BFILE_LAST_ERROR_CODE + 11)
#define FILE_INVALID_FORMAT     (BFILE_LAST_ERROR_CODE + 12)
// The value can't be set at this position
#define FILE_VALUE_ERROR        (BFILE_LAST_ERROR_CODE + 13)

// Edition modes
//
enum EDIT_MODE{
    EDIT_MODE_CREATION = 0, // Creation of a grid
    EDIT_MODE_MANUAL = 1    // Try to solve the grid
};

// Compelxity for created sudoku grids
//  The value correspond to the count of clues
//
enum GRID_COMPLEXITTY{
    COMPLEXITY_EASY = 33,
    COMPLEXITY_MEDIUM = 26,
    COMPLEXITY_HARD = 22
};

#define COMPLEXITY_BLOCKED_MAX  4

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

//   sudoku : Edition and/or resolution of a single sudoku grid
//
class sudoku{
public:

    // Constructions
    sudoku();
    sudoku(sudoku& original);

    // Destruction
    ~sudoku(){
        _freeSoluce();
    }

    // setElments() : set elements of the grid
    //
    //  @elements : list of elements
    //
    void setElements(element* elements);

    // setScreenRect() : Screen dimensions
    //
    //  rect : pointer to rect containing new dimensions
    //
    void setScreenRect(const RECT* rect);

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
    void revert(){
        _revertFrom(INDEX_MIN);
    }

    // create() : Create a new sudoku
    //
    //  @complexity : Complexity level in {}
    //
    //  @return : # of clues (ie of non empty elements) or -1 on error
    //
    int create(uint8_t complexity);

    //
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
    //  @mode : Edition mode,
    //          can be SUDOKU_MODE_CREATION or SUDOKU_MODE_MANUAL
    //
    //  @return : true if grid has been modified or false if unchanged
    //
    bool edit(uint8_t mode);
#endif // #ifdef DEST_CASIO_CALC

    // findObviousValues() : Find all the obvious values
    //
    //  @return : #obvious values found
    //
    uint8_t findObviousValues();

    // resolve() : Find a solution for the current grid
    //
    //  The algorithm will go forward to seek value and backward each
    //  time a value can't be found at a given pos.
    //  We going backward, if position reaches sPos (if not NULL)
    //  or exit the grid, the method end with no solution.
    //
    //  @mDuration : points to an int that will receive duration
    //              of solving process in ms. Can be NULL
    //  @soluce : Pointer to a table to copy the solution into. If NULL
    //              or if no soluce is found no copy is done
    //
    //  @return : true if a solution was found
    //
    bool resolve(int* mDuration = NULL, int8_t** soluce = NULL);

    // multipleSolutions() : Check wether a grid has one or many solutions
    //
    //  This method doesn't seek for all possible solutions since it stops
    //  when no soluce is found or at the second one
    //
    //  @return : 0 if the grid has no solution, 1 if a unique solution has
    //            been found -1 if many solutions may be founded (2 at least)
    //
    int multipleSolutions();

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
    bool findNextStartPos(position &start, int8_t& currentVal);

private:

    // _create() : Create a new sudoku
    //
    //  @complexity : Complexity level in {}
    //
    //  @return : # of clues (ie of non empty elements)
    //
    uint8_t _create(uint8_t complexity);

    //
    // Checks
    //

    // _checkValue() : Can we put the value at the given position ?
    //
    //  @pos : position
    //  @value : Check the given value at this 'position'
    //
    //  @return : true if the given value is valid at the given position
    //
    bool _checkValue(position& pos, uint8_t value);

    // _checkLine() : Can we put the value at the current line ?
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

    // _checkAndSet() : Try to  put the value at the given position
    //
    //  @pos : position
    //  @value : value to put
    //  @mode : edition mode
    //
    //  @return : -1 if can't be changed, 0 if changed an existing value,
    //          1 if new value set
    int _checkAndSet(position& pos, uint8_t value, uint8_t mode);

    //
    // Drawings
    //

#ifdef DEST_CASIO_CALC
    // _drawBackground() : Draw background and the grid's borders
    //
    void _drawBackground();

    // _drawContent() : Draw all the elements
    //
    void _drawContent();
#endif // #ifdef DEST_CASIO_CALC

    // _drawSingleElement : draw a single element of the grid
    //
    //  @pos: Position of the element
    //  @bkColour : background colour
    //  @txtColour : text colour
    //
    void _drawSingleElement(position pos, int bkColour, int txtColour);

    // _drawHypotheses() : Draw hypotheses list
    //
    void _drawHypotheses();

    //
    //   Search for obvious values
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
    bool _resolve(position* sPos);

    // _findObviousValues() :
    //  Search and set all the possible obvious values in the grid
    //
    //  @return the # of values found (and set)
    //
    uint8_t _findObviousValues();

    // _checkObviousValue() : Is there an obvious value
    //  for the given position ?
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

    // _onEditCheckSudoku() : Check wether grid can be solved
    //
    void _onEditCheckSudoku();

    //
    // Resolving
    //

    // _onHelp() : Help the user to solve the current grid
    //
    //  A new clue element is shown
    //
    //  @menu : Edit sub-menu
    //
    //  @return : index of added element or -1 if none were added
    //
    int8_t _onManualHelp(menuBar& menu);

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

    // _createEditMenu() : Create sub-menu displayed on edition mode
    //
    //  @menu : Menu bar to modifiy
    //  @editMode : Edition mode (game or creation)
    //
    void _createEditMenu(menuBar& menu, uint8_t editMode);

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
    int _elementTxtColour(position& pos, uint8_t editMode, bool selected);

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
    static bool _ownMenuItemsDrawings(PMENUBAR const bar,
            PMENUITEM const  item, RECT* const anchor, int style);

    // _onManualAccept() : Accept all the hypothese's values
    //
    //  When accepted, all elements with the current hyp. colour
    //  will have their colour changed to the previous selected col.
    //
    //  @menu : Edit submenu
    //
    //  @return : Count of elements concerned
    //
    uint8_t _onManualAccept(menuBar& menu);

    // _onManualReject() : Reject all the hypothese's values
    //
    //  When rejected, all elements with the given hyp. colour
    //  will be cleared and set with the @colTo colour
    //
    //  @colTo : Dest. colour
    //
    //  @return : Count of elements concerned
    //
    uint8_t _onManualReject(int colTo = HYP_NO_COLOUR);

    // _onChangeHypothese() : Change coloured hyp.
    //
    //  @menu : Hyp. colours menu
    //  @newHypID : New colour index (in menu)
    //
    void _onChangeHypothese(menuBar& menu, int newHypID);

    //
    // Utilities
    //

    // _revertFrom : Return to the "original" state
    //
    //  All values "after" @from prosition will be set to 0 if
    //  nor 'original'
    //
    //  @from : starting position (included)
    //
    void _revertFrom(uint8_t from);

    //  _copyElements() : Create a copy of current elements table
    //
    //  Elements values will be : > 0 for orignal values, 0 for empty
    //  values et < 0 for 'found' values
    //
    //  @dest : destination table
    //
    void _copyElements(int8_t* dest);

    //  _freeSoluce() : Free the memory allocated for a solution
    //
    void _freeSoluce(void);

    // Members
private:
    element elements_[LINE_COUNT * ROW_COUNT];  // grid as a one-dim. table
    tinySquare tSquares_[TINY_COUNT];	// Access to elements in tinySquares
    int8_t *soluce_;   // A solution for the current grid

    // Position & dims of screen
    // {x Grid, yGrid, "screen" width , "screen" height}
    RECT screen_;

    // Hypotheses & help
    //
    typedef struct{
        uint8_t menuID; // ID in the menu
        int colour;     // Associated colour
    }HYPOTHESE;

    HYPOTHESE hypotheses_[HYP_COUNT];
    int hypID_; // Current hyp. index in the table
    uint8_t helpClues_;
};

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

#endif // __S_SOLVER_SUDOKU_h__

// EOF
