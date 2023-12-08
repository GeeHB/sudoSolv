//---------------------------------------------------------------------------
//--
//--	File	: keyboard.cpp
//--
//--	Author	: Jérôme Henry-Barnaudière - GeeHB
//--
//--	Project	:
//--
//---------------------------------------------------------------------------
//--
//--	Description:
//--
//--			Implementation of keyboard object
//--
//---------------------------------------------------------------------------

#include "keyboard.h"

// Key event in the queue
//
uint keyboard::getKey(){
    uint key(KEY_CODE_NONE);

#ifdef DEST_CASIO_CALC
    key_event_t evt;
    evt = pollevent();
    if (evt.type == KEYEV_DOWN){
        key = evt.key;

        // Modifiers
        //
        if (key == KEY_SHIFT){
            if (!_isSet(MOD_SHIFT)){
                _set(MOD_SHIFT);
            }
        }
        else{
            if (key == KEY_ALPHA){
                if (!_isSet(MOD_ALPHA)){
                    _set(MOD_ALPHA);
                }
            }
        }
    }
    else{
        if (evt.type == KEYEV_UP){
            key = evt.key;

            // Modifiers
            //
            if (key == KEY_SHIFT){
                _unSet(MOD_SHIFT);
            }
            else{
                if (key == KEY_ALPHA){
                    _unSet(MOD_ALPHA);
                }
            }
        }

        key = KEY_CODE_NONE;
    }
#else
    key = getchar();
    mod_ = MOD_NONE;
#endif // #ifdef DEST_CASIO_CALC

    // return the pressed key code
    return key;
}

// EOF
