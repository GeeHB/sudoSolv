//----------------------------------------------------------------------
//--
//--    window.h
//--
//--            Definition of window object - Show a "window" on
//--            top of the screen
//--
//----------------------------------------------------------------------

#ifndef ___WINDOW_h__
#define ___WINDOW_h__    1

#include "casioCalcs.h"

#ifdef DEST_CASIO_CALC
#include <gint/display.h>
#else
#include <iostream>
#endif // #ifdef DEST_CASIO_CALC

#define _GEEHB_WINDOW_VER_      "0.1.2"

#define WIN_BORDER_WIDTH        2

// "style" of a window
#define WIN_STYLE_SBORDER       1   // Simple border
#define WIN_STYLE_DBORDER       2   // double border
#define WIN_STYLE_VCENTER       4   // Center window vertically
#define WIN_STYLE_HCENTER       8   // Center horizontally
#define WIN_STYLE_CENTER        (WIN_STYLE_VCENTER | WIN_STYLE_HCENTER)

#define WIN_STYLE_DEFAULT       (WIN_STYLE_DBORDER | WIN_STYLE_CENTER)

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

//  window object - A basic window on top of the screen
//

class window{
public:

    typedef struct _winInfo{

        // Construction
        _winInfo();

        // clear
        void clear();

        // Copy
        void copy(_winInfo& src);

        char*   title;
        int     style;
        RECT    pos;
        int     bkColour;
        int     borderColour;
        int     textColour;
    }winInfo;

    // Construction
    window();

    // Destruction
    ~window(){
        close();
    }

    // create() : Creation of a window
    //
    //  @info : Information concerning the new window
    //
    //  @return : true if created else false
    //
    bool create(winInfo& info);

    // close() : Close the current window
    //
    void close();

    // update() : Update the screen
    //
    void update();

    //
    // Helpers
    //

    // drawText() : Draw a line of text (in window coordinates)
    //
    //  @text : pointer to the text to draw
    //  @x, @y : coordinates of text relative to top left corner of the window
    //          if < 0, the text will be centered
    //          (horizontally for x  <0, vertically if y < 0)
    //  @tCol : text colour. If is equal to -1, the default text colour
    //           will be used
    //  @bCol : background colour. If is equal to -1,
    //          the default ground colour will be used
    //
    void drawText(const char* text, int x, int y,
                int tCol = -1, int bCol = -1);

    // win2Screen() : Convert window (x, y) into screen (x,y)
    //
    //  @coord : Coordinates to convert
    //
    void win2Screen(POINT& coord){
        coord.x+=client_.x;
        coord.y+=client_.y;
    }

private:
#ifdef DEST_CASIO_CALC
    // _rect2Window() : Convert a rect. struct to a window struct
    //
    void _rect2Window(RECT& rect, struct dwindow& win){
        win = {rect.x, rect.y, rect.x + rect.w, rect.y+rect.h};
    }

    // _drawBorder() : Draw a single border
    //
    void _drawBorder(struct dwindow& dest);
#endif // #ifdef DEST_CASIO_CALC

protected:
    // Members
    //
    bool        activated_; // Is the window in place ?
    winInfo     infos_;     // Informations concerning this window
    RECT        client_;    // Client area in screen coordinates
};

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

#endif // __WINDOW_h__

// EOF
