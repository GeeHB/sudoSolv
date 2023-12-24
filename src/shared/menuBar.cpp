//---------------------------------------------------------------------------
//--
//--    menuBar.cpp
//--
//--        Implementation of menuBar object  - A bar of menu (or a submenu)
//--
//---------------------------------------------------------------------------

#include "menuBar.h"

#include <cstdlib>
#include <cstring>

#ifndef DEST_CASIO_CALC
#include <iostream>
using namespace std;
#define KEY_F1      'a'
#define KEY_F6      'y'
#define KEY_EXIT    'x'
#else
// Background image
//
extern bopti_image_t g_backMenu;

#define MENU_BACK_IMG_WIDTH 12
#define MENU_BACK_IMG_HEIGHT 10

#endif // #ifndef DEST_CASIO_CALC

// Construction
//
menuBar::menuBar(){
    // Intializes members
    visible_ = &current_; // show current bar
    rect_ = {0, CASIO_HEIGHT - MENUBAR_DEF_HEIGHT - 1, CASIO_WIDTH, MENUBAR_DEF_HEIGHT};

    _clearMenuBar(&current_);
}

// Dimensions
//
bool menuBar::setHeight(uint16_t barHeight){
    rect_ = {0, CASIO_HEIGHT - barHeight - 1, CASIO_WIDTH, barHeight};
    update();
    return true;
}

// Drawings
//
void menuBar::update(){
    if (NULL == visible_){
        return;
    }

    RECT anchor = {rect_.x, rect_.y, MENUBAR_DEF_ITEM_WIDTH, rect_.h};   // First item's rect
    PMENUITEM item(nullptr);
    for (uint8_t index(0); index < MENU_MAX_ITEM_COUNT; index++){
        item = visible_->items[index];
#ifdef DEST_CASIO_CALC
        _drawItem(&anchor, item);
#else
        if (item){
            cout << "|" << ((item->state == ITEM_SELECTED)?">" : " ");
            cout << item->text;
            cout << ((item->state == ITEM_SELECTED)?"<" : " ");
            cout << "|";
        }
        else{
            cout << "|     |";
        }
#endif // DEST_CASIO_CALC

        anchor.x += anchor.w;   // Next item's position
    } // for

#ifdef DEST_CASIO_CALC
    dupdate();
#else
    cout << endl;
#endif // #ifdef DEST_CASIO_CALC
}

//  removeItemByID() : Remove an item
//      Remove the item menu or the submenu with the given ID
//
//  @id : Item's id
//
//  @return : true if the item has been successfully removed
//
bool menuBar::removeItemByID(int id){
    PMENUBAR bar(NULL);
    uint8_t index(0);
    PMENUITEM item(NULL);

    // Item exists ?
    if ((item = _findItemByID(&current_, id, &bar, &index))){
        // Yes => remove it
        return _removeItemByIndex(bar, index);
    }

    return false;
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
bool menuBar::activate(int id, bool activated){
    PMENUITEM item = _findItemByID(&current_, id);
    if (item){
        // Found an item with this ID
        bool inactive = (item->state == ITEM_INACTIVE);
        if (inactive == activated){
            item->state = (activated?ITEM_DEFAULT:ITEM_INACTIVE);   // change item's state
            return true;
        }
    }

    return false;
}

// handleKeyboard() : Handle the keyboard events
//
// @return : MENUACTION struct containing info about item selected b user
//
MENUACTION menuBar::handleKeyboard(){
    uint key(0);
    uint8_t kID(0);
    keyboard kb;
    PMENUITEM item(NULL);
    MENUACTION ret = {0, MOD_NONE, ACTION_KEYBOARD};
    bool readKeyboard(true);

    if (readKeyboard){
        key = kb.getKey();

        // a menu key pressed ?
        if (key >= KEY_F1 && key <= KEY_F6){
            kID = (key - KEY_F1);

            // Associated item
            if (kID < MENU_MAX_ITEM_COUNT &&
                (item = visible_->items[kID]) &&
                item->state != ITEM_INACTIVE){
                // A sub menu ?
                if (item->status == ITEM_STATUS_SUBMENU){
                    if (item->subMenu){
                        visible_ = (MENUBAR*)item->subMenu; // new "visible" menu
                        _selectIndex(-1, false, false);
                        update();
                    }
                }
                else{
                    if (IDM_RESERVED_BACK == item->id){
                        visible_ = visible_->parent;    // Return to previous menu
                        _selectIndex(-1, false, false);
                        update();
                    }
                    else{
                        // a selectable item ...
                        ret.value = item->id;
                        ret.type = ACTION_MENU;
                        readKeyboard = false;

                        // select the item
                        if (kID != visible_->selIndex){
                            _selectIndex(kID, true, true);
                        }
                    }
                }
            }

#ifndef DEST_CASIO_CALC
            update();
#endif // #ifndef DEST_CASIO_CALC

        } // if in [KEY_F1, KEY_F6]
        else{
            switch (key){

                // Back to prev. menu (if exists)
                case KEY_EXIT:
                    if (visible_ && visible_->parent){
                        visible_ = visible_->parent;
                        _selectIndex(-1, false, false);
                        update();
                    }
                    else{
                    }

                    break;

                default :
                    ret.value = key;
                    ret.type = ACTION_KEYBOARD;
                    readKeyboard = false;
                    break;
            }
        }
    }

    // Return keyboard event
    ret.modifier = kb.modifier();
    return ret;
}

//
// Menu bars
//

//  _addSubMenu() : Add a sub menu
//
//  @container : menubar container of the submenu
//  @index : index of position of the submenu
//  @submenu : submenu to add
//  @id : ID associated to the menu
//  @text : Submenu text
//
//  @return : true if sub menu is added
//
bool menuBar::_addSubMenu(PMENUBAR container, uint8_t index, PMENUBAR subMenu, int id, const char* text){
    size_t len(0);
    if (!container || !subMenu ||
        index >= MENU_MAX_ITEM_COUNT ||
        NULL != container->items[index] ||
        _findItemByID(container, id) ||
        container->itemCount == (MENU_MAX_ITEM_COUNT - 1) ||
        ! text || !(len = strlen(text))){
        return false;
    }

    // Create a copy of the menu bar
    PMENUBAR sub = _copyMenuBar(subMenu);
    if (NULL == sub){
        return false;
    }

    // Create item
    PMENUITEM item = _createItem(id, text, ITEM_DEFAULT);
    if (NULL == item){
        _freeMenuBar(sub, true);
        return false;
    }

    item->status = ITEM_STATUS_SUBMENU;
    item->subMenu = sub;
    sub->parent = container;

    container->items[index] = item;    // Add an item pointing to the sub menu
    container->itemCount++;

    return true;
}

// _clearMenuBar() : Empty a menu bar
//
//  @bar : menu bar the clear
//
void menuBar::_clearMenuBar(PMENUBAR bar){
    if (bar){
        bar->itemCount = 0;
        bar->selIndex = -1;     // No item is selected
        bar->parent = NULL;
        memset(bar->items, 0, sizeof(PMENUITEM) * MENU_MAX_ITEM_COUNT);
    }
}

//  _copyMenuBar() : Make a copy of a menu bar
//
//  All contained items and sub menus will be copied.
//
//  @source : Pointer to the source or NULL on error
//
//  @return : Pointer to the new copy
//
PMENUBAR menuBar::_copyMenuBar(PMENUBAR source){
    PMENUBAR bar(NULL);
    if (source && (bar = (PMENUBAR)malloc(sizeof(MENUBAR)))){
        _clearMenuBar(bar);
        bar->itemCount = source->itemCount;
        bar->parent = source->parent;
        uint8_t maxItem(MENU_MAX_ITEM_COUNT-1);
        PMENUITEM sitem, nitem;
        for (uint8_t index(0); index < maxItem; index++){
            nitem = _copyItem(bar, (sitem = source->items[index]));
            bar->items[index] = nitem;  // simple item copy
        }

        // In sub menus last right item is used to return to parent menu
        bar->items[MENU_MAX_ITEM_COUNT-1] = _createItem(IDM_RESERVED_BACK, STR_RESERVED_BACK, ITEM_DEFAULT);
    }
    return bar;
}

//  _freeMenuBar() : Free memory used by a bar
//
//  @bar : Pointer to the bar to be released
//  @freeAll : Free bar as well ?
//
void menuBar::_freeMenuBar(PMENUBAR bar, bool freeAll){
    if (bar){
        PMENUITEM item(NULL);
        for (uint8_t index(0); index < MENU_MAX_ITEM_COUNT; index++){
            if ((item = bar->items[index])){
                // A submenu ?
                if ((item->status & ITEM_STATUS_SUBMENU) && item->subMenu){
                    _freeMenuBar((PMENUBAR)item->subMenu, true);
                }

                free(item); // free the item
            }
        }

        if (freeAll){
            free(bar);  // free the bar
        }
    }
}

//
// Menu items
//

//  _findItemByID() : Find an item in the given bar
//
//  @bar : menu bar containing to search item in
//  @id : id of the searched item
//
//  @containerBar : pointer to a PMENUBAR. when not NULL, if item is ofund,
//                  containerBar will point to the bar containing the item
//  @pIndex : when not NULL, will point to the Item'index in its menubar.
//
//  @return : pointer to the item if found or NULL
//
PMENUITEM menuBar::_findItemByID(PMENUBAR bar, int id, PMENUBAR* containerBar, uint8_t* pIndex){
    if (bar){
        PMENUITEM item(NULL), sItem(NULL);
        for (uint8_t index = 0; index < MENU_MAX_ITEM_COUNT; index++){
            if ((item = bar->items[index])){
                if (item->status & ITEM_STATUS_SUBMENU){
                    // in a sub menu ?
                    if ((sItem = _findItemByID((PMENUBAR)item->subMenu, id, containerBar, pIndex))){
                        return sItem;   // Found in a sub menu
                    }
                }
                else{
                    if (item->id == id){
                        if (containerBar){
                            (*containerBar) = bar;
                        }

                        if (pIndex){
                            (*pIndex) = index;
                        }

                        return item;    // found
                    }
                }
            }
        }
    } // if (bar)

    // not found
    return NULL;
}

//  _addItem() : Add an item to a menu bar
//
//  @bar : Pointer to the container bar
//  @index : Index (position) in the menu bar
//  @id : Item ID
//  @text : Item text
//  @state : Item's initial state
//
//  @return : true if the item has been added
//
bool menuBar::_addItem(PMENUBAR bar, uint8_t index, int id, const char* text, int state){
    size_t len(0);
    if (!bar ||
        index >= MENU_MAX_ITEM_COUNT ||
        NULL != bar->items[index] ||
        _findItemByID(bar, id) ||    // this ID is already handled
        current_.itemCount == (MENU_MAX_ITEM_COUNT - 1) ||
        ! text || !(len = strlen(text))){
        return false;
    }

    PMENUITEM item = _createItem(id, text, state);
    if (item){
        // Add to the menu
        if (IDM_RESERVED_BACK == id){
            bar->items[MENU_MAX_ITEM_COUNT-1] = item;
        }
        else{
            bar->items[index] = item;
            bar->itemCount++;
        }

        // Successfully added
        return true;
    }

    return false;
}

//  _createItem() : creae a new menu item
//
//  @id : Item's id
//  @text : Menu item text
//  @state : Item's initial state
//
//  @return : pointer to the new created if valid or NULL
//
PMENUITEM menuBar::_createItem(int id, const char* text, int state){
    size_t len(strlen(text));
    MENUITEM* item(NULL);
    if ((item = (PMENUITEM)malloc(sizeof(MENUITEM)))){
        item->id = id;
        item->state = state;
        item->status = ITEM_STATUS_DEFAULT;
        if (len > ITEM_NAME_LEN){
            strncpy(item->text, text, ITEM_NAME_LEN);
            item->text[ITEM_NAME_LEN] = '\0';
        }
        else{
            strcpy(item->text, text);
        }

        item->subMenu = NULL;
    }

    return item;
}

//  _copyItem() : Make a copy of an item
//
//  @bar : Destination menu bar container
//  @source : Pointer to the source item
//
//  @return : pointer to the copied item or NULL
//
PMENUITEM menuBar::_copyItem(PMENUBAR bar, PMENUITEM source){
    PMENUITEM item(NULL);
    if (source && (item = (PMENUITEM)malloc(sizeof(MENUITEM)))){
        item->id = source->id;
        item->state = source->state;
        item->status = source->status;
        strcpy(item->text, source->text);;

        if (source->status & ITEM_STATUS_SUBMENU){
            item->subMenu = _copyMenuBar((PMENUBAR)source->subMenu);
            ((PMENUBAR)(item->subMenu))->parent = bar;  // attach it to the new bar
        }
        else{
            item->subMenu = NULL;
        }
    }
    return item;
}

//  _removeItemByIndex() : Remove an item
//      Remove the item menu or the submenu at the given index
//
//  @bar : Bar containing item to remove
//  @index : Item's index
//
//  @return : true if the item has been successfully removed
//
bool menuBar::_removeItemByIndex(PMENUBAR bar, uint8_t index){
    PMENUITEM item(NULL);
    if (bar && index < MENU_MAX_ITEM_COUNT && (item = bar->items[index])){
         // A sub menu ?
         if (item->status & ITEM_STATUS_SUBMENU){
            _freeMenuBar((PMENUBAR)item->subMenu, true);
         }

         // Position is free
         free(item);
         bar->items[index] = NULL;
         bar->itemCount--;

         // done
         return true;
    }

    // not removed
    return false;
}

//  _selectIndex() : Select an item by index in the current bar
//
//  @index : index of menu item to select or unselect
//          if equal to -1, unselect the currently selected item
//  @selected : true if item is to be selected
//  @redraw : when true, item and previously (un)selected item are drawn in their
//            new states
//
//  @return : true if item is selected
//
bool menuBar::_selectIndex(int8_t index, bool selected, bool redraw){
    int8_t sel;
    if (index >= MENU_MAX_ITEM_COUNT ||
        index == (sel = getSelectedIndex())){
        return false;   // Nothing to do
    }

    if (-1 == index){
        index = sel;
        selected = false;
    }

    // Select or unselect an item
    if (index >= 0){
        // Item
        PMENUITEM item = visible_->items[index];
        if (NULL == item){
            return false;
        }

        if (selected){
            item->state = ITEM_SELECTED;
            visible_->selIndex = index;

            // unselect prev.
            if (-1 != sel && (item = visible_->items[sel])){
                item->state = ITEM_UNSELECTED;
            }
        }
        else{
            item->state = ITEM_UNSELECTED;
            visible_->selIndex = -1;
        }
    }

    // Update screen ?
    if (redraw){
        RECT anchor = {rect_.x, rect_.y, MENUBAR_DEF_ITEM_WIDTH, rect_.h};
        if (-1 != sel){
            anchor.x+=(sel * anchor.w);
            _drawItem(&anchor, visible_->items[sel]);
        }

        if (-1 != index){
            anchor.x=(index * anchor.w + rect_.x);
            _drawItem(&anchor, visible_->items[index]);
        }
    }

    // Done
    return true;
}

//  _drawItem() : Draw an item
//
//  @anchor : Position of the item in screen coordinates
//  @item : Pointer to a MENUITEM strcut containing informations about item to draw
//
void menuBar::_drawItem(const RECT* anchor, const MENUITEM* item){
#ifdef DEST_CASIO_CALC
    bool selected(false);

    // Draw back ground
    drect(anchor->x, anchor->y, anchor->x + anchor->w - 1, anchor->y + anchor->h - 1, COLOUR_WHITE);

    if (item){
        selected = (ITEM_SELECTED == item->state);

        // Text
        if (item->text){
            int x,y, w, h;

            // Text is centerd
            dsize(item->text, NULL, &w, &h);
            y = anchor->y + (anchor->h - h) / 2;

            if (IDM_RESERVED_BACK == item->id){
                // New text position
                x = anchor->x + (anchor->w - w - MENU_BACK_IMG_WIDTH - 2) / 2;

                // Draw icon on left of text
                dimage(x, y, &g_backMenu);
                x+=(MENU_BACK_IMG_WIDTH + 2);
            }
            else{
                x = anchor->x + (anchor->w - w) / 2;
            }

            // text too large ?

            // draw the text
            dtext(x, y, selected?ITEM_COLOUR_SELECTED:((item->state == ITEM_INACTIVE)?ITEM_COLOUR_INACTIVE:ITEM_COLOUR_UNSELECTED), item->text);
        }

        // frame
        if (selected){
            dline(anchor->x, anchor->y, anchor->x, anchor->y + anchor->h - 2, COLOUR_BLACK); // Left
            dline(anchor->x+1, anchor->y + anchor->h - 1,
                anchor->x + anchor->w -1 - ITEM_ROUNDED_DIM, anchor->y + anchor->h - 1, COLOUR_BLACK);  // bottom
            dline(anchor->x + anchor->w -1 - ITEM_ROUNDED_DIM, anchor->y + anchor->h - 1,
                anchor->x + anchor->w - 1, anchor->y + anchor->h - 1 - ITEM_ROUNDED_DIM, COLOUR_BLACK);  // bottom
            dline(anchor->x + anchor->w - 1, anchor->y,
                anchor->x + anchor->w - 1, anchor->y + anchor->h - 1 - ITEM_ROUNDED_DIM, COLOUR_BLACK);   // right
        }
    } // if (item)

    if (!selected){
        dline(anchor->x, anchor->y, anchor->x + anchor->w -1, anchor->y, COLOUR_BLACK);
    }
#endif // #ifdef DEST_CASIO_CALC
}

// EOF