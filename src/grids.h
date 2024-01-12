//----------------------------------------------------------------------
//--
//--    grids.h
//--
//--        Definition of grids object - List of grid files in a folder
//--
//--        The object works as a rudimentary DB
//--        where each file has a numeric ID
//--
//----------------------------------------------------------------------

#ifndef __S_SOLVER_GRIDS_h__
#define __S_SOLVER_GRIDS_h__    1

#include "consts.h"
#include "shared/bFile.h"

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

class grids{
public:

    // Construction
    grids();

    // Destruction
    ~grids();

    // # files in the folder
    int size(){
        return count_;  //return files_.size();
    }

    // pos() : Get current position index in list
    //
    //  @return : index or -1 if list is empty
    //
    int pos(){
        return index_;
    }

    // findPos() : Find the index (position) of a file in the internal list
    //
    //  @UID: File's ID
    //
    //  @return : index of file in liste or -1
    //
    int findPos(int UID);

    // setPos() : Set current position index in list
    //
    //  @index: new position index
    //
    //  @return : index or -1 if list is empty
    //
    int setPos(int index);

    // browse() : browse folder and fill list with file names
    //
    //  @return : count of elements read or -1 if error
    //
    int browse();

    // currentFileName() : Get surrent filename
    //
    //  @fName : current filename
    //
    //  @return : true if valid 
    //
    bool currentFileName(FONTCHARACTER fName);

    // addFileName() : Add a filename to the folder's content list
    //
    //  @fName : filename to add
    //
    //  @return : true if successfully added
    //
    bool addFileName(const FONTCHARACTER fName);

    // nextFile() : Get next file name
    //
    //  @fName : fullname of the next file
    //
    //  @return : true if the next file name is valid
    //
    bool nextFile(FONTCHARACTER fName);

    // prevFile() : Get previous file name
    //
    //  @fName : fullname of the previous file
    //
    //  @return : true if the previous file name is valid
    //
    bool prevFile(FONTCHARACTER fName);

    // newFileName() : Generates a unique file name
    //
    //  @fName : new FQN
    //  @UID : pointer to int who will receive file's unique ID
    //          can be NULL
    //
    //  @return : true if name is valid
    //
    bool getNewFileName(FONTCHARACTER fName, int* UID);

    // deleteFile() : Delete current file
    //
    //  @return : true if deleted
    //
    bool deleteFile();

    // Internal methods
private:

    // _addFile() - Add file to the list
    //
    //  @fileName : file to add (with no folder)
    //
    //  @return : true if added
    //
    bool _addFile(const FONTCHARACTER fileName);

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
    //  @freeList : free list as well ?
    //
    void __vector_clear(bool freeList);

    //
    // Utils
    //

    // _nextFileID()- Get next ID for a new file
    //
    //  @return : numeric value or -1 on error
    //
    int _nextFileID();

    // _freeFileName : free the menory used by the FNAME struct.
    //
    //  @pItem : pointer to item to be freed
    //
    void _freeFileName(PFNAME pItem);

    // __fileName2i()- Convert a fully qualified filename to int
    //
    //  @src : Filename to convert
    //
    //  @return : numeric value or -1 on error
    //
    int __fileName2i(const FONTCHARACTER src);

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
