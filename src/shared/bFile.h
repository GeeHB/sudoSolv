//----------------------------------------------------------------------
//--
//--	bFile.h
//--
//--		Definition of bFile object
//--
//--		This release isn't fully tested
//--
//--		Missing :
//--                    - whence in read ops
//--					- BFile_Ext_Stat
//--					- seek API
//--
//----------------------------------------------------------------------

#ifndef __GEE_TOOLS_B_FILE_h__
#define __GEE_TOOLS_B_FILE_h__      1

#define VERSION_B_FILE_OBJECT       0.3.3

#ifdef DEST_CASIO_CALC
#include <gint/gint.h>
#include <gint/bfile.h>

typedef uint16_t const * FONTCHARACTER;
typedef int SEARCHHANDLE;
#else
#include <cstdio>
#include <fstream>
#include <iostream>
#include <dirent.h>
#include <filesystem>       // to create a folder
namespace fs = std::filesystem;

typedef char* FONTCHARACTER;
typedef DIR* SEARCHHANDLE;

// Entry types
//
#define BFile_File    1
#define BFile_Folder  5

// Access modes
#define BFile_ReadOnly      0x01
#define BFile_WriteOnly     0x02
#define BFile_ReadWrite     (BFile_ReadOnly|BFile_WriteOnly)
#define BFile_Share         0x80


struct BFile_FileInfo
{
	uint16_t index;
	uint16_t type;
	uint32_t file_size;
	/* Data size (file size minus the header) */
	uint32_t data_size;
	/* Is 0 if the file is complete */
	uint32_t property;
	/* Address of first fragment (do not use directly) */
	void *address;
};

#define BFile_Type_Directory  0x0000
#define BFile_Type_File       0x0001
#define BFile_Type_Addin      BFile_Type_File
#define BFile_Type_Eact       0x0003
#define BFile_Type_Language   0x0004
#define BFile_Type_Bitmap     0x0005
#define BFile_Type_MainMem    0x0006
#define BFile_Type_Temp       0x0007
#define BFile_Type_Dot        0x0008
#define BFile_Type_DotDot     0x0009
#define BFile_Type_Volume     0x000a
#define BFile_Type_Archived   BFile_Type_File
#endif // #ifndef DEST_CASIO_CALC

// Max length in char of a path
//
#define BFILE_MAX_PATH          127

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

    // exist() : Check wether the file or folder exists
    //
    // @return : true if the file or folder exists
    //
    bool exist(FONTCHARACTER fName);

    // isOpen() : Is the file already open ?
    //
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
    // @size : Pointer to file size if type is BFile_File,
    //			use NULL otherwise
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

    // rename() : Rename or move a file
    //
    //  @oldPath : name of the file to rename
    //  @newPath : destination path
    //
    //  @return : file successfully renamed ?
    //
    bool rename(FONTCHARACTER oldPath, FONTCHARACTER newPath);

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

    // findFirst(): Search the storage memory for paths
    //	matching a pattern
    //
    //  @pattern    FONTCHARACTER glob pattern
    //  @sHandle    Will receive search handle
    //  @foundFile  Will receive FONTCHARACTER path of matching entry
    //  @fileInfo   Will receive metadata of matching entry
    //
    //  @return :  True on success
    //
    bool findFirst(const FONTCHARACTER pattern,
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
    bool findNext(SEARCHHANDLE sHandle,
			FONTCHARACTER foundFile, struct BFile_FileInfo *fileInfo);

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
    // Utilities
    //

    // FC_str2FC() : Convert a char* to FC format
    //
    //  @src : string to convert
    //  @dest : destination buffer
    //
    //  @return : pointer to a FONTCHARACTER
    //
    static bool FC_str2FC(const char* src, FONTCHARACTER dest);

    // FC_FC2str() : Convert a string from FC format to char*
    //
    //  @src : FC to convert
    //  @dest : destination buffer
    //
    //  @return : pointer to a FONTCHARACTER
    //
    static bool FC_FC2str(const FONTCHARACTER src, char* dest);

    // FC_cpy() : Copy a FONTCHARACTER to another FONTCHARACTER
    //
    //  @dest : pointer to a destination string (already allocated)
    //  @src : pointer to the source string
    //
    //  @return : pointer to the copy if done  NULL on error
    //
    static FONTCHARACTER FC_cpy(FONTCHARACTER dest, const FONTCHARACTER src);

    // FC_cat() : Cancatenate 2 FONTCHARACTER
    //
    //  @dest : pointer to the destination string
    //  @add : pointer to the FONTCHARACTER to cancatenate
    //
    //  @return : pointer to the destination string if done  NULL on error
    //
    static FONTCHARACTER FC_cat(FONTCHARACTER dest, const FONTCHARACTER add);

    // FC_dup() : Duplicate a FONTCHARACTER
    //
    //  @fName : filename to copy
    //
    //  @return : pointer to the copy or NULL on error
    //
    static FONTCHARACTER FC_dup(const FONTCHARACTER src);

    // FC_len() : length of a fileName in "char"
    //
    //  @fName : pointer to the FONTCHARACTER
    //
    //  @return : size of fName (O on error)
    //
    static size_t FC_len(const FONTCHARACTER fName);

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
