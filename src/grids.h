//---------------------------------------------------------------------------
//--
//--    grids.h
//--
//--        Definition of grids object - List of grid files stored on "disk"
//--
//---------------------------------------------------------------------------

#ifndef __S_SOLVER_GRIDS_h__
#define __S_SOLVER_GRIDS_h__    1

#include "consts.h"
#include "shared/bFile.h"

#include <vector>
using namespace std;

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

class grids{
public:

    // Construction
    grids(const FONTCHARACTER folder);

    // Destruction
    ~grids(){}

    // # files in folder
    size_t size();

    // Access
    bool nextFile(FONTCHARACTER fName);
    bool prevFile(FONTCHARACTER fName);

    // File management
    bool newFileName(FONTCHARACTER folder);
    bool deleteFile();  // current

    // Members
protected:
    vector<int> files_;
    int         current_;   // Current file ID
};

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

#endif // __S_SOLVER_GRIDS_h__

// EOF
