//---------------------------------------------------------------------------
//--
//--	File	: scrCapture.h
//--
//--	Author	: Jérôme Henry-Barnaudière - GeeHB
//--
//--	Project	:
//--
//---------------------------------------------------------------------------
//--
//--	Description:
//--
//--			Definition of scrCapture - Screen capture using fxlink
//--
//---------------------------------------------------------------------------

#ifndef __CASIO_CALC_SCREEN_CAPTURE_h__
#define __CASIO_CALC_SCREEN_CAPTURE_h__    1

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

//---------------------------------------------------------------------------
//--
//-- scrCapture object - Screen capture using fxlink command -[ fxlink -iw ]
//--
//---------------------------------------------------------------------------

class scrCapture{
public:
    // Construction
    scrCapture(){
        set_ = false;
    }

    // Destruction
    ~scrCapture(){
        remove();
    }

    // Already installed ?
    bool isSet(){
        return set_;
    }

    // Install
    bool install();

    // Remove
    void remove();

private:
    // Members
    bool    set_;   // Is the capture on ?
};

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

#endif // __CASIO_CALC_SCREEN_CAPTURE_h__

// EOF
