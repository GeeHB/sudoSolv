//----------------------------------------------------------------------
//--
//--    menuBar.h
//--
//--        Definition of menuBar object - A bar of menu (or a submenu)
//--
//----------------------------------------------------------------------

#ifndef __MENU_BAR_h__
#define __MENU_BAR_h__    1

#include "casioCalcs.h"
#include "keyboard.h"

#define _GEEHB_MENU_VER_        "0.5.1"

#define MENU_MAX_ITEM_COUNT     6   // ie. "F" buttons count

//
// Dimensions in pixels
//
#define MENUBAR_DEF_HEIGHT      22
#define MENUBAR_DEF_ITEM_WIDTH  (CASIO_WIDTH / MENU_MAX_ITEM_COUNT)

#define ITEM_NAME_LEN           10  // Max length of item name

// Image (in bar) dimensions
#define MENU_IMG_WIDTH          12
#define MENU_IMG_HEIGHT         12

#define ITEM_ROUNDED_DIM         4

//
// Item pos is a menu bar
//
#define MENU_POS_LEFT           0
#define MENU_POS_RIGHT          (MENU_MAX_ITEM_COUNT-1)

//
// Item state - could be any combination of :
//
#define ITEM_STATE_DEFAULT          0
#define ITEM_STATE_SELECTED         1
#define ITEM_STATE_INACTIVE         2
#define ITEM_STATE_CHECKED          4
#define ITEM_STATE_NO_BACK_BUTTON   8

//
// Item status - could be any combination of :
//
#define ITEM_STATUS_DEFAULT     0
#define ITEM_STATUS_TEXT        1   // Item's text is valid
#define ITEM_STATUS_SUBMENU     2   // Open a sub menu on "click"
#define ITEM_STATUS_KEYCODE     4   // item's ID is a key code
#define ITEM_STATUS_CHECKBOX    8   // Item is a checkbox
#define ITEM_STATUS_OWNERDRAWN  16  // Use own callback for item drawing

// Drawing styles for ownerdraw function
//
#define MENU_DRAW_BACKGROUND    1
#define MENU_DRAW_TEXT          2
#define MENU_DRAW_IMAGE         4
#define MENU_DRAW_BORDERS       8

// Checkbox state
//
enum CHECKBOX_STATE{
    ITEM_ERROR = -1,     // Unknown state or not a checkbox
    ITEM_UNCHECKED = 0,
    ITEM_CHECKED = 1
};

// A few helpers ...

// Draw content (but not background)
#define MENU_DRAW_CONTENT (MENU_DRAW_TEXT|MENU_DRAW_IMAGE|MENU_DRAW_BORDERS)

// Draw all, by default
#define MENU_DRAW_ALL    (MENU_DRAW_CONTENT | MENU_DRAW_BACKGROUND)

//
// Reserved menu item ID
//
#define IDM_RESERVED_BACK       0xFFFB  // Return to parent menu
#ifdef DEST_CASIO_CALC
#define STR_RESERVED_BACK       "back"
#endif // #ifdef DEST_CASIO_CALC

//
// Menu colours
//

// ID of colours (for {set/get}Colour methods
//
enum MENU_COLOURS_ID{
    TXT_SELECTED = 0,
    TXT_UNSELECTED = 1,
    TXT_INACTIVE = 2,
    ITEM_BACKGROUND = 3,
    ITEM_BACKGROUND_SELECTED = 4,       // for future use
    ITEM_BORDER = 5,
    COL_COUNT = 6
};

// Default colours
//
#define ITEM_COLOUR_SELECTED    COLOUR_BLUE
#define ITEM_COLOUR_UNSELECTED  COLOUR_DK_GREY
#define ITEM_COLOUR_INACTIVE    COLOUR_LT_GREY
#define ITEM_COLOUR_BACKGROUND  COLOUR_WHITE
#define ITEM_COLOUR_BORDER      COLOUR_BLACK

// A single item
//
typedef struct _menuItem{
    int id;
    int state;
    int status;
    char text[ITEM_NAME_LEN + 1];
    void* subMenu;  // if item is a submenu, points to the submenu
    int ownerData;  // Can ba anything ...
} MENUITEM,* PMENUITEM;

// A menu bar
//
typedef struct _menuBar{
    uint8_t itemCount;
    int8_t selIndex;
    _menuBar* parent;
    void* pDrawing;    // Pointer to ownerdraw callback
    PMENUITEM items[MENU_MAX_ITEM_COUNT];
    int colours[COL_COUNT];
} MENUBAR, * PMENUBAR;

// Action to perform
//
typedef struct _menuAction{
    int value;
    uint modifier;
    uint8_t type;
} MENUACTION;

// Ownerdraw's function prototype
//
typedef bool (*MENUDRAWINGCALLBACK)(
            PMENUBAR const,     // MenuBar containing the item
            PMENUITEM const,    // Item to draw
            RECT* const,        // Drawing rect for item
            int style);         // Elements (in item) to draw

// Types of actions
//
enum MENU_ACTION{
    ACTION_MENU = 0,    // value is a menu ID
    ACTION_KEYBOARD = 1 // value is a keycode
};

// Types of search modes
//
enum MENU_SEARCH_MODE{
    SEARCH_BY_ID = 0,
    SEARCH_BY_INDEX = 1
};

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

//   menuBar : A bar of menu (or submenu)
//
class menuBar{
public:

    // Construction
    menuBar();

    // Destruction
    ~menuBar(){
        _freeMenuBar(&current_, false);
    }

    //
    // Ownerdraw method
    //

    // setMenuDrawingCallBack() : Set function for ownerdraw drawings
    //
    // When an item has the ITEM_STATUS_OWNERDRAWN status bit set, the
    // ownerdraw callback function will be called each time the menubar
    // needs to redraw the item
    //
    // @pF : Pointer to the callback function or NULL if no ownerdraw
    //
    // @return : pointer to the default drawing function
    //
    MENUDRAWINGCALLBACK setMenuDrawingCallBack(MENUDRAWINGCALLBACK pF);

    // getColour() : Get the colour used for item's drawings in the
    //              active menu bar
    //
    //  @index : index of the colour to retreive
    //
    //  @return : colour or -1 if error
    //
    int getColour(uint8_t index){
        return ((index>=COL_COUNT)?-1:visible_->colours[index]);
    }

    // setColour() : Change the colour used for item's drawings in the
    //              active menu bar
    //
    //  @index : index of the colour to change
    //  @colour : new colour value
    //
    //  @return : previous colour or -1 if error
    //
    int setColour(uint8_t index, int colour);

    //
    // Dimensions
    //

    //  getHeight() : Get menu bar height
    //
    //  @return : Height of menu bar in pixels
    //
    uint16_t getHeight(){
        return rect_.h;
    }

    //  getRect() : Get bounding rect of current menu bar
    //
    void getRect(RECT& barRect);

    //  setHeight() : change menu bar height
    //
    //  @barHeight : New height in pixels
    //  @updateBar : Update the menubar ?
    //
    //  @return : true if hieght has changed
    //
    bool setHeight(uint16_t barHeight, bool updateBar = true);

    //  update() : Update the menu bar
    //
    //  All items will be drawn according to their state et status
    //
    void update();

    //
    // Items & subBars
    //

    //  size() : count of items or sub menus in the bar
    //
    //  @return : The count of items or sub menus
    //
    uint8_t size(){
        return current_.itemCount;
    }

    //  addSubMenu() : Add a sub menu
    //
    //  @index : Index (position) in the menu bar
    //  @submenu : menubar corresponding to the new submenu
    //  @id : ID associated to the menu
    //  @text : Submenu text
    //  @state : Menu item initial state
    //  @status : initial status of submenu
    //
    //  @return : pointer the created item or NULL
    //
    PMENUITEM addSubMenu(uint8_t index, menuBar* const subMenu,
            int id, const char* text, int state = ITEM_STATE_DEFAULT,
            int status = ITEM_STATUS_DEFAULT){
        return _addSubMenu(&current_, index,
                        (PMENUBAR)subMenu, id, text, state, status);
    }

    //  appendSubMenu() : Append a sub menu
    //
    //  @submenu : menubar corresponding to the new submenu
    //  @id : ID associated to the menu
    //  @text : Submenu text
    //  @state : Menu item initial state
    //  @status : initial status of submenu
    //
    //  @return : pointer the created item or NULL
    //
    PMENUITEM appendSubMenu(const menuBar* subMenu, int id,
                        const char* text, int state = ITEM_STATE_DEFAULT,
                        int status = ITEM_STATUS_DEFAULT){
        return _addSubMenu(&current_, current_.itemCount,
                            (PMENUBAR)subMenu, id, text, state, status);
    }

    //  addItem() : Add an item to the current menu bar
    //
    //  @index : Index (position) in the menu bar
    //  @id : Item ID
    //  @text : Item text
    //  @state : Item's initial state
    //  @status : Item's status
    //
    //  @return : pointer the created item or NULL
    //
    PMENUITEM addItem(uint8_t index, int id, const char* text,
                int state = ITEM_STATE_DEFAULT,
                int status = ITEM_STATUS_DEFAULT){
        return _addItem(&current_, index, id, text, state, status);
    }

    //  addCheckbox() : Add an item to the current menu bar
    //
    //  @index : Index (position) in the menu bar
    //  @id : Item ID
    //  @text : Item text
    //  @state : Item's initial state
    //  @status : Item's status
    //
    //  @return : pointer the created item or NULL
    //
    PMENUITEM addCheckBox(uint8_t index, int id, const char* text,
                int state = ITEM_STATE_DEFAULT,
                int status = ITEM_STATUS_DEFAULT){
        return _addItem(&current_, index, id, text,
                    state, ITEM_STATUS_CHECKBOX | status);
    }

    //  appendItem() : Append an item to the current menu bar
    //
    //  @id : Item ID
    //  @text : Item text
    //  @state : Item's initial state
    //  @status : Item's status
    //
    //  @return : pointer the created item or NULL
    //
    PMENUITEM appendItem(int id, const char* text,
                    int state = ITEM_STATE_DEFAULT,
                    int status = ITEM_STATUS_DEFAULT){
        return _addItem(&current_, current_.itemCount, id, text, state, status);
    }

    //  appendCheckbox() : Append a checkbox to the current menu bar
    //
    //  @id : Item ID
    //  @text : Item text
    //  @state : Item's initial state
    //  @status : Item's status
    //
    //  @return : pointer the created item or NULL
    //
    PMENUITEM appendCheckbox(int id, const char* text,
                    int state = ITEM_STATE_DEFAULT,
                    int status = ITEM_STATUS_DEFAULT){
        return _addItem(&current_, current_.itemCount, id, text,
                        state,  status | ITEM_STATUS_CHECKBOX);
    }

    // isMenuItemChecked() : Check wether a checkbox is in the checked state
    //
    //  @id : checkbox item id
    //  @searchMode : type of search (SEARCH_BY_ID or SEARCH_BY_INDEX)
    //
    //  return : ITEM_CHECKED if the item is checked, ITEM_UNCHECKED
    //          if the item is not cheched and ITEM_ERROR on error
    //          (invalid id, not a check box, ...)
    //
    int isMenuItemChecked(int id, int searchMode = SEARCH_BY_ID);

    // checkMenuItem() : Check or uncheck a menu item checkbox
    //
    //  @id : checkbox item id
    //  @searchMode : type of search (SEARCH_BY_ID or SEARCH_BY_INDEX)
    //  @checkState : ITEM_CHECKED if item should be checked or ITEM_UNCHECKED
    //
    //  return : ITEM_CHECKED it item is checked, ITEM_UNCHECKED if not checked
    //           and ITEM_ERROR on error
    //
    int checkMenuItem(int id, int searchMode = SEARCH_BY_ID,
                    int check = ITEM_CHECKED);

    //  removeItem() : Remove an item from the current menu bar
    //      Remove the item menu or the submenu
    //
    //  @searchedID : Item's ID or index
    //  @searchMode : if SEARCH_BY_ID, searchedID is the ID of item
    //                if SEARCH_BY_INDEX serachedID is the index in the menu bar
    //
    //  @return : true if the item has been successfully removed
    //
    bool removeItem(int searchID, int searchMode){
        return _removeItem(&current_, searchID, searchMode);
    }

    //
    // Selection & activation
    //

    //  selectByIndex() : Select an item by index in the current bar
    //
    //  @index : index of menu item to select or unselect
    //          if equal to -1, unselect the currently selected item
    //  @selected : true if item is to be selected
    //  @redraw : Redraw the item ?
    //
    //  @return : true if item is selected
    //
    bool selectByIndex(int8_t index, bool selected = true,
                    bool redraw = false){
        return _selectByIndex(index, selected, redraw);    // Redraw items
    }

    //  getSelectedIndex() : Index of selected item in the current bar
    //
    //  @return : Index of item selected or -1 if none
    //
    int8_t getSelectedIndex(){
        return (visible_?visible_->selIndex:current_.selIndex);
    }

    //  getItemState() : Get the state of an item
    //
    //  @searchedID : ID of searched item
    //  @searchMode : type of search (SEARCH_BY_ID or SEARCH_BY_INDEX)
    //
    //  @return : Item's state or -1 if error
    //
    int8_t getItemState(int searchedID, int searchMode);

    //  activateItem() : Activate or deactivate an item
    //
    //  When an item is deactivated, it can't be called by the user
    //
    //  @searchedID : Menu item's ID
    //  @searchMode : Type of search (SEARCH_BY_ID or SEARCH_BY_INDEX)
    //  @activated : true if item must be activated
    //
    //  @return : true if activation state changed
    //
    bool activateItem(int searchedID, int searchMode, bool activated = true);
    bool activate(int searchedID, int searchMode, bool activated = true){
        return activateItem(searchedID, searchMode, activated);
    }

    //
    // Item access and modifications
    //

    bool getItem(int searchID, int searchMode, PMENUITEM* pItem);
    bool setItem(int searchID, int searchMode, PMENUITEM pItem, int Mask);

    //  findItem() : Find an item in the menu bar and its submenus
    //
    //  @searchedID : id or index of the searched item
    //  @searchMode : Type of search (SEARCH_BY_ID or SEARCH_BY_INDEX)
    //
    //  @containerBar : pointer to a PMENUBAR. when not NULL,
    //          if item is found, containerBar will point to the bar
    //          containing the item
    //  @pIndex : when not NULL, will point to the item's index in its menu
    //
    //  @return : pointer to the item if found or NULL
    //
    PMENUITEM findItem(int searchedID, int searchMode,
                PMENUBAR* containerBar = NULL,
                uint8_t* pIndex = NULL){
        return _findItem(&current_, searchedID, searchMode,
                        containerBar, pIndex);
    }

    //  freeMenuItem() : Free memory used by a menu item
    //
    //  @item : Pointer to the menu item to be released
    //
    void freeMenuItem(PMENUITEM item);

    // Conversion
    operator PMENUBAR() const{
        return (MENUBAR*)(&current_);
    }

    // handleKeyboard() : Handle keyboard events
    //
    // @return : MENUACTION struct containing info
    //          about item selected b user
    //
    MENUACTION handleKeyboard();

    // showParentBar() : Return to parent menubar if exists
    //
    //  @updateBar : update the menu ?
    //
    void showParentBar(bool updateBar = true);

    //  defDrawItem() : Draw an item
    //
    // @bar : Pointer to the bar containing the item to be drawn
    //  @item : Pointer to a MENUITEM strcut containing informations
    //          concerning the item to draw
    //  @anchor : Position of the item in screen coordinates
    //  @style : Drawing style ie. element(s) to draw
    //
    //  @return : False on error(s)
    //
    static bool defDrawItem(MENUBAR* const bar, MENUITEM* const item,
                            RECT* const anchor, int style);

    // State & status
    //
    static bool isBitSet(int value, int bit){
        return (bit == (value & bit));
    }
    static int setBit(int& value, int bit){
        value |= bit;
        return value;
    }
    static int removeBit(int& value, int bit){
        value = value & ~bit;
        return value;
    }

    // Internal methods
private:

    //
    // menu bars
    //

    //  _addSubMenu() : Add a sub menu
    //
    //  @container : menubar container of the submenu
    //  @index : index of position of the submenu
    //  @submenu : submenu to add
    //  @id : ID associated to the menu
    //  @text : Submenu text
    //  @state : initial state of submenu
    //  @status : initial status of submenu
    //
    //  @return : pointer the created item or NULL
    //
    PMENUITEM _addSubMenu(PMENUBAR const container, uint8_t index,
                PMENUBAR const subMenu, int id, const char* text,
                int state, int status);

    // _clearMenuBar() : Empty a menu bar
    //
    //  @bar : menu bar the clear
    //
    void _clearMenuBar(PMENUBAR bar);

    //  _copyMenuBar() : Make a copy of a menu bar
    //
    //  All contained items and sub menus will be copied.
    //
    //  @source : Pointer to the source
    //  @noBackButton : Don't add a "back button" at the last position
    //                  to return to previous menu
    //
    //  @return : Pointer to the new copy or NULL on error
    //
    PMENUBAR _copyMenuBar(PMENUBAR const source, bool noBackButton);

    //  _freeMenuBar() : Free memory used by a bar
    //
    //  @bar : Pointer to the bar to be released
    //  @freeAll : Free bar as well ?
    //
    void _freeMenuBar(PMENUBAR bar, bool freeAll);

    //
    // menu items management
    //

    //  _addItem() : Add an item to a menu bar
    //
    //  @bar : Pointer to the container bar
    //  @index : Index (position) in the menu bar
    //  @id : Item ID
    //  @text : Item text
    //  @state : Item's initial state
    //  @status : Item's status
    //
    //  @return : pointer the created item or NULL
    //
    PMENUITEM _addItem(PMENUBAR const bar, uint8_t index, int id,
                    const char* text, int state = ITEM_STATE_DEFAULT,
                    int status = ITEM_STATUS_DEFAULT);

    //  _createItem() : creae a new menu item
    //
    //  @id : Item's id
    //  @text : Menu item text
    //  @state : Item's initial state
    //  @status : Item's status
    //
    //  @return : pointer to the new created if valid or NULL
    //
    PMENUITEM _createItem(int id, const char* text, int state, int status);

    //  _copyItem() : Make a copy of an item
    //
    //  @bar : Destination menu bar container
    //  @source : Pointer to the source item
    //
    //  @return : pointer to the copied item or NULL
    //
    PMENUITEM _copyItem(PMENUBAR const bar, PMENUITEM const source);

    //  _findItem() : Find an item in the given bar
    //
    //  @bar : menu bar containing to search item in
    //  @searchedID : id or index of the searched item
    //  @searchMode : Type of search (SEARCH_BY_ID or SEARCH_BY_INDEX)
    //
    //  @containerBar : pointer to a PMENUBAR. when not NULL,
    //          if item is found, containerBar will point to the bar
    //          containing the item
    //  @pIndex : when not NULL, will point to the item's index in its menu
    //
    //  @return : pointer to the item if found or NULL
    //
    PMENUITEM _findItem(PMENUBAR const bar, int searchedID,
                int searchMode, PMENUBAR* containerBar = NULL,
                uint8_t* pIndex = NULL);

    //  _removeItem() : Remove an item from the current menu bar
    //      Remove the item menu or the submenu
    //
    //  @bar : menu bar in wich the item is to be searched
    //  @searchedID : Item's ID or index
    //  @searchMode : if SEARCH_BY_ID, searchedID is the ID of item
    //                if SEARCH_BY_INDEX searchedID is the index in the menu bar
    //
    //  @return : true if the item has been successfully removed
    //
    bool _removeItem(PMENUBAR const bar, int searchedID, int searchMode);

    //  _selectByIndex() : Select an item by index in the current bar
    //
    //  @index : index of menu item to select or unselect
    //  @selected : true if item is to be selected
    //  @redraw : when true, item and previously (un)selected
    //              item are drawn in their new states
    //
    //  @return : true if item is selected
    //
    bool _selectByIndex(int8_t index, bool selected, bool redraw);

    //  _drawItem() : Draw an item
    //
    //  @item : Pointer to a MENUITEM strcut containing informations
    //          concerning the item to draw
    //  @anchor : Position of the item in screen coordinates
    //
    //  @return : False on error(s)
    //
    bool _drawItem(PMENUITEM const item, RECT* const anchor);

    // Members
private:
    MENUBAR     current_;   // Active bar
    RECT        rect_;      // Bar position and dims
    PMENUBAR    visible_;   // Visible menu bar
};

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

#endif // __MENU_BAR_h__

// EOF
