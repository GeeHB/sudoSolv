//----------------------------------------------------------------------
//--
//--    scrCapture.cpp
//--
//--    Implementation of scrCapture - Screen capture using fxlink
//--
//----------------------------------------------------------------------

#include "scrCapture.h"

#ifndef NO_CAPTURE
#include <gint/display.h>
#include <gint/usb.h>
#include <gint/usb-ff-bulk.h>
#endif // #ifndef NO_CAPTURE

// Construction
//
scrCapture::scrCapture(){
    set_ = false;
    paused_ = false;
}

// Destruction
//
scrCapture::~scrCapture(){
    remove();
}

// Already installed ?
//
bool scrCapture::isSet(){
    return set_;
}

// Install
//
bool scrCapture::install(){
#ifdef NO_CAPTURE
    paused_ = false;
    return (set_ = true);
#else
    if (set_){
        return false;    // Already installed
    }

    // List of interfaces
    usb_interface_t const *intf[] = { &usb_ff_bulk, NULL };

    // Waiting for connexion
    usb_open((usb_interface_t const **)&intf, GINT_CALL_NULL);
    usb_open_wait();

    // Set the hook
    dupdate_set_hook(GINT_CALL(usb_fxlink_videocapture, 0));

    paused_ = false;
    return (set_ = true);
#endif // #ifdef NO_CAPTURE
}

// Remove
//
void scrCapture::remove(){
#ifndef NO_CAPTURE
    if (set_){
        dupdate_set_hook(GINT_CALL_NULL);
    }
#endif // #ifndef NO_CAPTURE

    set_ = false;
    paused_ = false;
}

//
// Handle activity
//

// pause() : pause the capture
//
// @return : paused ?
//
bool scrCapture::pause(){
    if (set_ && !paused_){
        remove();
        paused_ = true;
    }
    else{
        paused_ = false;
    }

    return paused_;
}

// resume() : resume the capture
//
// @return : paused ?
//
bool scrCapture::resume(){
    if (!set_ && paused_){
        if (install()){
            paused_ = false;
        }
    }

    return paused_;
}

// EOF
