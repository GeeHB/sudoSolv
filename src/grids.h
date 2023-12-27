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

/*
#include <vector>
using namespace std;
*/

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

class grids{
public:

    // Construction
    grids();

    // Destruction
    ~grids(){
        __vector_clear();
    }

    // # files in folder
    int size(){
        //return files_.size();
        return count_;
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

    // _browse() : browse folder and fill list with file names
    //
    void _browse();

    // Add file
    bool _addFile(FONTCHARACTER fileName);

    // Generate a name associated with an ID
    bool _ID2Name(uint16_t id, FONTCHARACTER folder);

    // Informations about a file
    //
    typedef struct _FNAME{
        FONTCHARACTER   fileName;   // FQN
        int ID;
    }FNAME,* PFNAME;

    //
    //  "vector" management
    //

    // __vector_append() : append an file item pointer to the list
    //
    //  This method automaticaly resizes the list
    //
    //  @file : pointer to the struct to add to the list
    //
    //  @return : true if succesfully added
    //
    bool __vector_append(PFNAME file);

    // __vector_resize() : resize the list
    //
    //  @return : true if succesfully resized
    //
    bool __vector_resize();

    // __vector_clear() : clear the list and its content
    //
    void __vector_clear();

    //
    // strings utils
    //

    // __fileName2i()- Convert a fully qualified filename to int
    //
    //  @src : Filename to convert
    //
    //  @return : numeric value or -1 on error
    //
    int __fileName2i(FONTCHARACTER src);

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

    // Members
private:

    char folder_[BFILE_MAX_PATH + 1];
    int index_;     // Index of current file in list (-1 if none)

    // We can't use vectors, so let's use my own "vector" like table
    // vector<PFNAME> files_;     // List of "file names"
    PFNAME* files_;     // List of files
    int     capacity_;  // max. size of buffer
    int     count_;     // Item count
};

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

#endif // __S_SOLVER_GRIDS_h__

// EOF
