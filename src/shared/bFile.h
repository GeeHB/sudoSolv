//----------------------------------------------------------------------
//--
//--    bFile.h
//--
//--        Definition of bFile object
//--
//--        This release isn't fully tested
//--
//--    Missing :
//--        - whence in read ops
//--        - BFile_Ext_Stat
//--        - seek API
//--
//----------------------------------------------------------------------

#ifndef __GEE_TOOLS_B_FILE_h__
#define __GEE_TOOLS_B_FILE_h__      1

#define _GEEHB_BFILE_VER_    "0.5.6"

#ifdef DEST_CASIO_CALC
#include <gint/gint.h>
#include <gint/bfile.h>

typedef uint16_t const * FONTCHARACTER;
typedef int SEARCHHANDLE;
#define BFILE_CHAR_ZERO 0x0000

#else
#include "bFileLocals.h"
#endif // #ifndef DEST_CASIO_CALC

// Max lengthof a path  in "FONTCHARACTER"
//
#define BFILE_MAX_PATH                  127

// Error codes specific to this class
//
enum BILE_CODE{
    BFILE_NO_ERROR = 0,
    BFILE_ERROR_INVALID_PARAMETERS = 1,   // BFile_IllegalParam ?
    BFILE_ERROR_FILE_NOT_OPENED = 2,
    BFILE_ERROR_FILE_OPENED = 3,
    BFILE_ERROR_INVALID_FILENAME = 4,
    BFILE_ERROR_MEMORY = 5,
    BFILE_LAST_ERROR_CODE = BFILE_ERROR_MEMORY
};

//
// FONTCHARACTER macros
//

// Clear an existing "string"
#define FC_EMPTY(fc) {fc[0] = BFILE_CHAR_ZERO;}

// Is the "string" empty ?
#define FC_ISEMPTY(fc) (!fc || BFILE_CHAR_ZERO == fc[0])

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

    // exist() : Check wether the file or the folder exists
    //
    // @return : true if the file or folder exists
    //
    bool exist(FONTCHARACTER const fName);

    // isOpen() : Is the file already open ?
    //
    //
    // @return : true if the object is open
    //
    bool isOpen();

    // size() : Get file size
    //
    // @return : size of current opened file or -1 on error
    //
    int size();

    // open() : Open an existing file
    //
    // @filename : name of the file to open
    // @access : Access mode to the file
    //
    // @return : file opened ?
    //
    bool open(FONTCHARACTER const filename, int access);
#ifdef DEST_CASIO_CALC
    bool open(char* const filename, int access);
#endif // #ifdef DEST_CASIO_CALC

    // create() : Create a file or a folder
    //
    // @fname : name of the file or folder to create (must not exist)
    // @type : Entry type (BFile_File or BFile_Folder)
    // @size : Pointer to file size if type is BFile_File,
    //          use NULL otherwise
    //
    // @return : file or folder successfully created ?
    //
    bool create(FONTCHARACTER const fname, int type, int *size);

    // createEx() : Create a file or a folder
    //
    //  Create the folder or the file. For file (type BFile_File),
    //  the method will try to open the file after creation
    //
    // @fname : name of the file or folder to create (must not exist)
    // @type : Entry type (BFile_File or BFile_Folder)
    // @size : Pointer to file size if type is BFile_File,
    //          use NULL otherwise
    // @access : Access mode for the file
    //
    // @return : file or folder successfully created
    //          - and openend for file ?
    //
    bool createEx(FONTCHARACTER const fname, int type,
                    int *size, int access);

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

    // rename() : Rename or move a file
    //
    //  @oldPath : name of the file to rename
    //  @newPath : destination path
    //
    //  @return : file successfully renamed ?
    //
    bool rename(FONTCHARACTER const oldPath,
                FONTCHARACTER const newPath);

    // remove() : Remove a file
    //
    // @filename : name of the file to remove
    //
    // @return : file successfully removed ?
    //
    bool remove(FONTCHARACTER const filename);

    // close() : Close the file
    //
    void close();

    // findFirst(): Search the storage memory for paths
    //  matching a pattern
    //
    //  @pattern    FONTCHARACTER glob pattern
    //  @sHandle    Will receive search handle
    //  @foundFile  Will receive FONTCHARACTER path of matching entry
    //  @fileInfo   Will receive metadata of matching entry
    //
    //  @return :  True on success
    //
    bool findFirst(FONTCHARACTER const pattern,
                SEARCHHANDLE*sHandle, FONTCHARACTER foundFile,
                struct BFile_FileInfo *fileInfo);

    // findNext(): Continue a search
    //
    // Continues the search for matches.
    //
    //  @sHandle : search handle
    //  @foundFile : next filename
    //  @fileinfo : file metadatas
    //
    //  @return : true if ok
    //
    bool findNext(SEARCHHANDLE sHandle, FONTCHARACTER foundFile,
                struct BFile_FileInfo *fileInfo);

    // findClose() :  Close a search handle
    //
    //  @sHandle : Search handle
    //
    //  @return : done ?
    //
    bool findClose(SEARCHHANDLE sHandle);

    // getLastError() : Get last error code
    //
    // @return : no error (0) or last error code
    //
    int getLastError(){
        return error_;
    }

    //
    // Utilities - conversions from/to FC to/from char*
    //

    // FC_str2FC() : Convert a char* to FC format
    //
    //  @src : string to convert
    //  @dest : destination buffer
    //
    //  @return : pointer to a FONTCHARACTER
    //
    static bool FC_str2FC(char* const src, FONTCHARACTER dest);

    // FC_FC2str() : Convert a string from FC format to char*
    //
    //  @src : FC to convert
    //  @dest : destination buffer
    //
    //  @return : pointer to a FONTCHARACTER
    //
    static bool FC_FC2str(FONTCHARACTER const src, char* dest);

    // FC_cpy() : Copy a FONTCHARACTER to another FONTCHARACTER
    //
    //  @dest : pointer to a destination string (already allocated)
    //  @src : pointer to the source string
    //
    //  @return : pointer to the copy if done  NULL on error
    //
    static FONTCHARACTER FC_cpy(FONTCHARACTER dest, FONTCHARACTER const src);

    // FC_cat() : Cancatenate 2 FONTCHARACTER
    //
    //  @dest : pointer to the destination string
    //  @add : pointer to the FONTCHARACTER to cancatenate
    //
    //  @return : pointer to the destination string if done  NULL on error
    //
    static FONTCHARACTER FC_cat(FONTCHARACTER dest, FONTCHARACTER const add);

    // FC_dup() : Duplicate a FONTCHARACTER
    //
    //  @fName : filename to copy
    //
    //  @return : pointer to the copy or NULL on error
    //
    static FONTCHARACTER FC_dup(FONTCHARACTER const src);

    // FC_len() : length of a fileName in "char"
    //
    //  @fName : pointer to the FONTCHARACTER
    //
    //  @return : size of fName (O on error)
    //
    static size_t FC_len(FONTCHARACTER const fName);

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
