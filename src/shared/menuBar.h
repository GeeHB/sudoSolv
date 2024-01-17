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

#define _GEEHB_MENU_VER_        "0.3.8"

#define MENU_MAX_ITEM_COUNT     6   // ie. "F" buttons count

// Dimensions in pixels
#define MENUBAR_DEF_HEIGHT      22
#define MENUBAR_DEF_ITEM_WIDTH  (CASIO_WIDTH / MENU_MAX_ITEM_COUNT)

#define ITEM_NAME_LEN           10  // Max length of item name

// Item pos is a menu bar
//
#define MENU_POS_LEFT           0
#define MENU_POS_RIGHT          (MENU_MAX_ITEM_COUNT-1)

// Item state - any combination of ...
//
#define ITEM_STATE_DEFAULT          0
#define ITEM_STATE_SELECTED         1
#define ITEM_STATE_INACTIVE         2
#define ITEM_STATE_CHECKED          4
#define ITEM_STATE_NO_BACK_BUTTON   8

// Item status
//
#define ITEM_STATUS_DEFAULT     0
#define ITEM_STATUS_TEXT        1
#define ITEM_STATUS_SUBMENU     2       // Open a sub menu on "click"
#define ITEM_STATUS_KEYCODE     4       // item's ID is a key code
#define ITEM_STATUS_CHECKBOX    8

// Reserved menu item ID
//
#define IDM_RESERVED_BACK       0xFFFB  // Return to parent menu
#ifdef DEST_CASIO_CALC
#define STR_RESERVED_BACK       "back"
#else
#define STR_RESERVED_BACK       "^ back ^"
#endif // #ifdef DEST_CASIO_CALC

// Item colors
#define ITEM_COLOUR_SELECTED     COLOUR_BLUE
#define ITEM_COLOUR_UNSELECTED   COLOUR_DK_GREY
#define ITEM_COLOUR_INACTIVE     COLOUR_LT_GREY

#define ITEM_ROUNDED_DIM         4

// A single item
//
typedef struct _menuItem{
    int id;
    int state;
    int status;
    char text[ITEM_NAME_LEN + 1];
    void* subMenu;      // if item is a submenu, points to the submenu
} MENUITEM,* PMENUITEM;

// A menu bar
//
typedef struct _menuBar{
    uint8_t itemCount;
    int8_t selIndex;
    _menuBar* parent;
    PMENUITEM items[MENU_MAX_ITEM_COUNT];
} MENUBAR, * PMENUBAR;

// Action to perform
//
typedef struct _menuAction{
    int value;
    uint modifier;
    uint8_t type;
} MENUACTION;

// Types of actions
//
#define ACTION_MENU         0     // value is a menu ID
#define ACTION_KEYBOARD     1     // value is a keycode

// Types of search modes
//
#define SEARCH_BY_ID        0
#define SEARCH_BY_INDEX     1

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
    //
    //  @return : true if hieght has changed
    //
    bool setHeight(uint16_t barHeight);

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
    //
    //  @return : true if sub menu is added
    //
    bool addSubMenu(uint8_t index, const menuBar* subMenu,
            int id, const char* text, int state = ITEM_STATE_DEFAULT){
        return _addSubMenu(&current_, index,
                        (PMENUBAR)subMenu, id, text, state);
    }

    //  appendSubMenu() : Append a sub menu
    //
    //  @submenu : menubar corresponding to the new submenu
    //  @id : ID associated to the menu
    //  @text : Submenu text
    //  @state : Menu item initial state
    //
    //  @return : true if sub menu is added
    //
    bool appendSubMenu(const menuBar* subMenu, int id,
                        const char* text, int state = ITEM_STATE_DEFAULT){
        return _addSubMenu(&current_, current_.itemCount,
                            (PMENUBAR)subMenu, id, text, state);
    }

    //  addItem() : Add an item to the current menu bar
    //
    //  @index : Index (position) in the menu bar
    //  @id : Item ID
    //  @text : Item text
    //  @state : Item's initial state
    //  @status : Item's status
    //
    //  @return : true if the item has been added
    //
    bool addItem(uint8_t index, int id, const char* text,
                int state = ITEM_STATE_DEFAULT,
                int status = ITEM_STATUS_DEFAULT){
        return _addItem(&current_, index, id, text, state, status);
    }

    //  appendItem() : Append an item to the current menu bar
    //
    //  @id : Item ID
    //  @text : Item text
    //  @state : Item's initial state
    //  @status : Item's status
    //
    //  @return : true if the item has been added
    //
    bool appendItem(int id, const char* text, int state = ITEM_STATE_DEFAULT,
                    int status = ITEM_STATUS_DEFAULT){
        return _addItem(&current_, current_.itemCount, id, text, state, status);
    }

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
    //
    //  @return : true if item is selected
    //
    bool selectByIndex(int8_t index, bool selected = true){
        return _selectByIndex(index, selected, true);    // Redraw items
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
    //
    //  @return : true if sub menu is added
    //
    bool _addSubMenu(const PMENUBAR container, uint8_t index,
                PMENUBAR subMenu, int id, const char* text, int state);

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
    PMENUBAR _copyMenuBar(const PMENUBAR source, bool noBackButton);

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
    //  @return : true if the item has been added
    //
    bool _addItem(const PMENUBAR bar, uint8_t index, int id,
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
    PMENUITEM _copyItem(const PMENUBAR bar, PMENUITEM source);

    //  _findItem() : Find an item in the given bar
    //
    //  @bar : menu bar containing to search item in
    //  @searchedID : id or index of the searched item
    //  @searchMode : Type of search (SEARCH_BY_ID or SEARCH_BY_INDEX)
    //
    //  @containerBar : pointer to a PMENUBAR. when not NULL,
    //          if item is found, containerBar will point to the bar
    //          containing the item
    //  @pIndex : when not NULL, will point to the Item'ID in its menu
    //
    //  @return : pointer to the item if found or NULL
    //
    PMENUITEM _findItem(const PMENUBAR bar, int searchedID,
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
    bool _removeItem(const PMENUBAR bar, int searchedID, int searchMode);

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
    //  @anchor : Position of the item in screen coordinates
    //  @item : Pointer to a MENUITEM strcut containing informations
    //          concerning the item to draw
    //
    void _drawItem(const RECT* anchor, const MENUITEM* item);

    // State & status
    //
    bool _isBitSet(int value, int bit){
        return (bit == (value & bit));
    }
    int _setBit(int& value, int bit){
        value |= bit;
        return value;
    }
    int _removeBit(int& value, int bit){
        value = value & ~bit;
        return value;
    }

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
