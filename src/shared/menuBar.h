//---------------------------------------------------------------------------
//--
//--    menuBar.h
//--
//--        Definition of menuBar object - A bar of menu (or submenu)
//--
//---------------------------------------------------------------------------

#ifndef __MENU_BAR_h__
#define __MENU_BAR_h__    1

#include "casioCalcs.h"
#include "keyboard.h"

#define MENU_MAX_ITEM_COUNT     6   // ie. "F" buttons count

// Dimensions in pixels
#define MENUBAR_DEF_HEIGHT      22
#define MENUBAR_DEF_ITEM_WIDTH  (CASIO_WIDTH / MENU_MAX_ITEM_COUNT)

#define ITEM_NAME_LEN           10  // Max length of item name

// Item state
//
#define ITEM_DEFAULT            0
#define ITEM_SELECTED           1
#define ITEM_UNSELECTED         ITEM_DEFAULT
#define ITEM_INACTIVE           2

// Item status
//
#define ITEM_STATUS_DEFAULT     0
#define ITEM_STATUS_SUBMENU     2       // Open a sub menu on "click"
#define ITEM_STATUS_KEYCODE     3       // item's ID is a key code

// Reserved menu item ID
//
#define IDM_SUBMENU             0xFFFA  // This item holds a submenu
#define IDM_BACK                0xFFFB  // Return to parent menu

#define STR_BACK                "^ back ^"

// item colors
#define ITEM_COLOUR_SELECTED     COLOUR_BLUE
#define ITEM_COLOUR_UNSELECTED   COLOUR_DK_GREY
#define ITEM_COLOUR_INACTIVE     COLOUR_GREY

#define ITEM_ROUNDED_DIM         4

// A single item
//
typedef struct _menuItem{
    int id;
    int state;
    int status;
    char text[ITEM_NAME_LEN + 1];
    void* subMenu;
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
    uint8_t type;
} MENUACTION;

// Type of actions
//
#define ACTION_MENU         0     // value is a menu ID
#define ACTION_KEYBOARD     1     // value is a keycode

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

    //  setHeight() : change menu bar height
    //
    //  @barHeight : New height in pixels
    //
    //  @return : true if hieght has changed
    //
    bool setHeight(uint16_t barHeight);

    //  getRect() : Get bounding rect of current menu bar
    //
    void getRect(RECT& barRect){
        barRect = {rect_.x, rect_.y, rect_.w, rect_.h};
    }

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
    //  @submenu : menubar corresponding to the new submenu
    //  @text : Submenu text
    //
    //  @return : true if sub menu is added
    //
    bool addSubMenu(const menuBar* subMenu, const char* text);

    //  createSubMenu() : Create an empty sub menu
    //
    //  @return : a pointer to the new menu object if created
    //
    //menuBar* createSubMenu();

    //  addItem() : Add an item to the current menu bar
    //
    //  @id : Item ID
    //  @text : Item text
    //  @state : Item's initial state
    //
    //  @return : true if the item has been added
    //
    bool addItem(int id, const char* text, int state = ITEM_DEFAULT);

    // Selection & activation
    //

    //  selectIndex() : Select an item by index in the current bar
    //
    //  @index : index of menu item to select or unselect
    //  @selected : true if item is to be selected
    //
    //  @return : true if item is selected
    //
    bool selectIndex(int8_t index, bool selected = true){
        return _selectIndex(index, selected, true);    // Redraw items
    }

    //  getSelectedIndex() : Index of selected item in the current bar
    //
    //  @return : Index of item selected or -1 if none
    //
    int8_t getSelectedIndex(){
        return (visible_?visible_->selIndex:current_.selIndex);
    }

    //  activate() : Activate or deactivate an item
    //
    //  When an item is deactivated, it can't be called by the user
    //
    //  @id : Menu item's ID
    //  @activated : true if item must be activated
    //
    //  @return : true if activation state changed
    //
    bool activate(int id, bool activated = true);

    // Access
    //
    operator PMENUBAR() const{
        return (MENUBAR*)(&current_);
    }

    // handleKeyboard() : Handle the keyboard events
    //
    // @return : MENUACTION struct containing info about item selected b user
    //
    MENUACTION handleKeyboard();

    // Internal methods
private:

    //
    // menu bars
    //

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
    //
    //  @return : Pointer to the new copy or NULL on error
    //
    PMENUBAR _copyMenuBar(PMENUBAR source);

    //  _freeMenuBar() : Free memory used by a bar
    //
    //  @bar : Pointer to the bar to be released
    //  @freeAll : Free bar as well ?
    //
    void _freeMenuBar(PMENUBAR bar, bool freeAll);

    //
    // menu items management
    //

    //  _findItemByID() : Find an item in the current bar
    //
    //  @bar : menu bar containing items
    //  @id : id of the searched item
    //
    //  @return : pointer to the item if found or NULL
    //
    PMENUITEM _findItemByID(PMENUBAR bar, int id);

    //  _createItem() : creae a new menu item
    //
    //  @id : Item's id
    //  @text : Menu item text
    //  @state : Item's initial state
    //
    //  @return : pointer to the new created if valid or NULL
    //
    PMENUITEM _createItem(int id, const char* text, int state);

    //  _copyItem() : Make a copy of an item
    //
    //  @bar : Destination menu bar container
    //  @source : Pointer to the source item
    //
    //  @return : pointer to the copied item or NULL
    //
    PMENUITEM _copyItem(PMENUBAR bar, PMENUITEM source);

    //  _selectIndex() : Select an item by index in the current bar
    //
    //  @index : index of menu item to select or unselect
    //  @selected : true if item is to be selected
    //  @redraw : when true, item and previously (un)selected item are drawn in their
    //            new states
    //
    //  @return : true if item is selected
    //
    bool _selectIndex(int8_t index, bool selected, bool redraw);

    //  _drawItem() : Draw an item
    //
    //  @anchor : Position of the item in screen coordinates
    //  @item : Pointer to a MENUITEM strcut containing informations about item to draw
    //
    void _drawItem(const RECT* anchor, const MENUITEM* item);

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
