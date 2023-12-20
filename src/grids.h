//---------------------------------------------------------------------------
//--
//--    grids.h
//--
//--        Definition of grids object - List of grid files stored on "disk"
//--
//--        The object works as a rudimentary DB
//--        where each file has a numeric ID
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
    ~grids();

    // # files in folder
    size_t size(){
        return files_.size();
    }

    // pos() : Get current position index in list
    //
    // @return : index or -1 if list is empty
    //
    int pos(){
        return index_;
    }

    // nextFile() : Get next file name
    //
    //  @fName : fullname of the next file
    //
    //  @return : true if the next file name is valid
    //
    bool nextFile(FONTCHARACTER& fName);

    // prevFile() : Get previous file name
    //
    //  @fName : fullname of the previous file
    //
    //  @return : true if the previous file name is valid
    //
    bool prevFile(FONTCHARACTER& fName);

    // File management
    bool newFileName(FONTCHARACTER fName);

    bool deleteFile();  // current

    // Internal methods
private:
    // Browsre grids'folder
    void _browse();

    // Generate a name associated with an ID
    bool _ID2Name(uint16_t id, FONTCHARACTER folder);

    // __atoi()- Convert a filename (without folder) to int
    //
    //  @src : Filename to convert
    //
    //  @return : numeric value or -1 on error
    //
    int __atoi(FONTCHARACTER src);

    // __itoa() : Transform a numeric value into a string
    //
    //  @num : Numeric value to transform
    //  @str : Pointer to output string
    //
    //  @return : pointer to formated string
    //
    char* __itoa(int num, char* str);

    // __strrev() : Reverse a string
    //
    //  @str : String to reverse
    //
    void __strrev(char *str);

#ifdef DEST_CASIO_CALC

#endif // DEST_CASIO_CALC

    // Members
private:

    typedef struct _FNAME{
        FONTCHARACTER   fileName;   // full name
        int ID;
    }FNAME,* PFNAME;

    vector<PFNAME> files_;     // List of "file names"
    int index_;     // Index of current file in list (-1 if none)
    int firstFreeID_;
};

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

#endif // __S_SOLVER_GRIDS_h__

// EOF
