//----------------------------------------------------------------------
//--
//--	File	: bFile.h
//--
//--	Author	: Jérôme Henry-Barnaudière - GeeHB
//--
//--	Project	:
//--
//----------------------------------------------------------------------
//--
//--	Description:
//--
//--			Definition of bFile object
//--
//--		This release isn't fully tested.
//--		At this time, it's only functionnal for FXCG50 calculator
//--
//--		Missing :
//--                    - whence in read ops
//--					- BFile_Ext_Stat
//--					- search API
//--					- seek API
//--
//----------------------------------------------------------------------

#ifndef __GEE_TOOLS_B_FILE_h__
#define __GEE_TOOLS_B_FILE_h__      1

#define VERSION_B_FILE_OBJECT       0.3

#ifdef DEST_CASIO_CALC
#include <gint/gint.h>
#include <gint/bfile.h>

typedef uint16_t const * FONTCHARACTER;
#else
#include <cstdio>
#include <fstream>
#include <iostream>

#include <filesystem>       // to create a folder
namespace fs = std::filesystem;

typedef char const * FONTCHARACTER;

// Entry types
//
#define BFile_File    1
#define BFile_Folder  5

// Access modes
#define BFile_ReadOnly      0x01
#define BFile_WriteOnly     0x02
#define BFile_ReadWrite     (BFile_ReadOnly|BFile_WriteOnly)
#define BFile_Share         0x80
#endif // #ifndef DEST_CASIO_CALC

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

//  bFile - A class for accessing file system
//
class bFile{
public:

    // Construction
    bFile();

    // Destruction
    ~bFile();

    // isOpen() : Is the file already open ?
    //
    //  Check wether file (or folder) exists
    //
    // @return : true if the object is valid
    //
    bool isOpen();

    // size() : Get file size
    //
    // @return : size of current opened file or an error code
    //
    int size();

    // open() : Open an existing file
    //
    // @filename : name of the file to open
    // @access : Access mode to the file
    //
    // @return : file opened ?
    //
    bool open(FONTCHARACTER filename, int access);

    // create() : Create a file or a folder
    //
    // @filename : name of the file or folder to create (must not exist)
    // @type : Entry type (BFile_File or BFile_Folder)
    // @size : Pointer to file size if type is BFile_File, use NULL otherwise
    //
    // @return : file successfully created ?
    //
    bool create(FONTCHARACTER filename, int type, int *size);

    // write() : Write data in the current file
    //
    // @data : Pointer to the data buffer
    // @size : Size in byte to write (should be an even number of bytes)
    //
    // @return : data written ?
    //
    bool write(void const *data, int even_size);

    // read() : Read data from the current file
    //
    // @data : Pointer to the destination buffer
    // @size : Size in byte to read
    // @whence : ?
    //
    // @return : # bytes read
    //
    int read(void *data, int size, int whence);

    // remove() : Remove a file
    //
    // @filename : name of the file to remove
    //
    // @return : file successfully removed ?
    //
    bool rename(FONTCHARACTER oldpath, FONTCHARACTER newpath);

    // remove() : Remove a file
    //
    // @filename : name of the file to remove
    //
    // @return : file successfully removed ?
    //
    bool remove(FONTCHARACTER filename);

    // close() : Close the file
    //
    void close();

    // getLastError() : Get last error code
    //
    // @return : no error (0) or last error code
    //
    int getLastError(){
        return error_;
    }

private:
#ifdef DEST_CASIO_CALC
    int fd_;        // File descriptor (>0 if valid)
#else
    std::fstream file_;
    std::string fileName_;
#endif // #ifdef DEST_CASIO_CALC

    int error_;     // Last error code
};

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

#endif // __GEE_TOOLS_B_FILE_h__

// EOF
