//----------------------------------------------------------------------
//--
//--    scrCapture.cpp
//--
//--    Implementation of scrCapture - Screen capture using fxlink
//--
//----------------------------------------------------------------------

#include "scrCapture.h"

#include <gint/display.h>
#include <gint/usb.h>
#include <gint/usb-ff-bulk.h>

// Construction
//
scrCapture::scrCapture(){
    set_ = false;
}

// Destruction
scrCapture::~scrCapture(){
    remove();
}

// Already installed ?
bool scrCapture::isSet(){
    return set_;
}

// Install
//
bool scrCapture::install(){
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
    return (set_ = true);
}

// Remove
void scrCapture::remove(){
    if (set_){
        dupdate_set_hook(GINT_CALL_NULL);
        set_ = false;
    }
}

// EOF
