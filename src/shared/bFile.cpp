//----------------------------------------------------------------------
//--
//--    bFile.cpp
//--
//-     Implementation of bFile object
//--
//--        This release isn't fully tested
//--
//--    Missing :
//--        - whence in read ops
//--        - BFile_Ext_Stat
//--        - seek API
//--
//----------------------------------------------------------------------

#include "bFile.h"

#ifndef DEST_CASIO_CALC
#include <malloc.h>
#endif // #ifndef DEST_CASIO_CALC

#include <cstring>
#include <cstdlib>

// Construction
//
bFile::bFile(){
#ifdef DEST_CASIO_CALC
    fd_ = 0;    // no file
#else
    fileName_ = "";
#endif // #ifdef DEST_CASIO_CALC

    error_ = BFILE_NO_ERROR;
}

// Destruction
//
bFile::~bFile(){
    close();
}

// exist() : Check wether the file or the folder exists
//
// @return : true if the file or folder exists
//
bool bFile::exist(FONTCHARACTER const fName){
    if (!FC_len(fName)){
        error_ = BFILE_ERROR_INVALID_PARAMETERS;
        return false;
    }

    bool exist(false);
    SEARCHHANDLE handle;
#ifdef DEST_CASIO_CALC
    uint16_t foundFile[BFILE_MAX_PATH+1];
#else
    char foundFile[BFILE_MAX_PATH+1];
#endif // DEST_CASIO_CALC
    struct BFile_FileInfo fileInfo;
    FC_EMPTY(foundFile);

    if (findFirst(fName, &handle, foundFile, &fileInfo)){
        findClose(handle);
        exist = true;   // The file or folder exists
        error_ = BFILE_NO_ERROR;
    }

    return exist;
}

// isOpen() : Is the file already open ?
//
//  Check wether file (or folder) exists
//  and last access error
//
//  @return : true if the object is open
//
bool bFile::isOpen(){
#ifdef DEST_CASIO_CALC
    return (fd_ && !error_);
#else
    return file_.is_open();
#endif // #ifdef DEST_CASIO_CALC
}

// size() : Get file size
//
// @return : size of current opened file or -1 on error
//
int bFile::size(){
    if (isOpen()){
#ifdef DEST_CASIO_CALC
        int ret = gint_world_switch(GINT_CALL(BFile_Size, fd_));
        if (ret < 0){
            error_ = ret;
            return -1;
        }

        error_ = BFILE_NO_ERROR;
        return ret;
#else
        file_.seekg (0, file_.end);
        return file_.tellg();
#endif // #ifdef DEST_CASIO_CALC
    }

    error_ = BFILE_ERROR_FILE_NOT_OPENED;
    return -1;
}

// open() : Open an existing file
//
// @filename : name of the file to open
// @access : Access mode to the file
//
// @return : file opened ?
//
bool bFile::open(FONTCHARACTER const filename, int access){
    if (access && !isOpen()){
#ifdef DEST_CASIO_CALC
        fd_ = gint_world_switch(GINT_CALL(BFile_Open, filename, access));
        if (fd_ < 0){
            error_ = fd_;
            fd_ = 0;
            return false;
        }
        else{
            error_ = BFILE_NO_ERROR;
            return true;
        }
#else
        std::ios_base::openmode mode;
        if (access & BFile_ReadOnly){
            mode = std::fstream::in | std::ifstream::binary;
        }
        else {
            mode = std::fstream::out | std::fstream::trunc;
        }

        file_.open(filename, mode);
        fileName_ = filename;

        // Done ?
        return isOpen();
#endif // #ifdef DEST_CASIO_CALC
    }


    error_ = (!access)?BFILE_ERROR_INVALID_PARAMETERS
                :BFILE_ERROR_FILE_OPENED;   // ... and should be close
    return false;
}

// create() : Create a file or a folder
//
// @fname : name of the file or folder to create (must not exist)
// @type : Entry type (BFile_File or BFile_Folder)
// @size : Pointer to file size if type is BFile_File,
//          use NULL otherwise
//
// @return : file successfully created ?
//
bool bFile::create(FONTCHARACTER const fname, int type, int *size){
    if (type == BFile_File){
        // File can't be open
        if (!isOpen()){
#ifdef DEST_CASIO_CALC
            error_ = gint_world_switch(GINT_CALL(BFile_Create,
                                fname, BFile_File, size));
            return (BFILE_NO_ERROR == error_);
#else
            // size is ignored
            if (open(fname, BFile_WriteOnly)){
                if (isOpen()){
                    return true;
                }
            }
#endif // #ifdef DEST_CASIO_CALC
        }
    }
    else{
        // Create a folder
#ifdef DEST_CASIO_CALC
            error_ = gint_world_switch(GINT_CALL(BFile_Create, fname,
                        BFile_Folder, size));
            return (BFILE_NO_ERROR == error_);
#else
            return fs::create_directory(fname);
#endif // BFile_File
    }

    // Unknown type
    error_ = BFILE_ERROR_INVALID_PARAMETERS;
    return false;
}

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
bool bFile::createEx(FONTCHARACTER const fname, int type,
                    int *size, int access){
    if (BFile_Folder == type){
        return create(fname, BFile_Folder, size);
    }

    // Try to create the file
    if (!create(fname, type, size)){
        return false;
    }

    // Created => open the file
    return open(fname, access);
}

// write() : Write data in the current file
//
// @data : Pointer to the data buffer
// @size : Size in byte to write (should be an even number of bytes)
//
// @return : data written ?
//
bool bFile::write(void const *data, int even_size){

    if (!data || !even_size){
        error_ = BFILE_ERROR_INVALID_PARAMETERS;
        return false;
    }

    if (!isOpen()){
        error_ = BFILE_ERROR_FILE_NOT_OPENED;
        return false;
    }

    bool done(true);
    int mySize(even_size);

#ifdef DEST_CASIO_CALC
    char* buffer = (char*)data;
#ifdef FX9860G
    if (0 != (even_size % 2)){
        if (NULL == (buffer = malloc(++mySize))){
            error_ = BFILE_ERROR_MEMORY;
            return false;
        }

        // Copy the buffer
        memcpy(buffer, data, even_size);

        // ... add a byte
        memcpy((void*)(buffer + even_size), 0x00, 1);
    }
#endif // #ifdef FX9860G
    error_ = gint_world_switch(GINT_CALL(BFile_Write, fd_,
                                (void*)buffer, mySize));
    done =  (error_ > 0);   // should have write something !
#ifdef FX9860G
    // Free the buffer ?
    if (0 != (even_size % 2)){
        free(buffer);
    }
#endif // #ifdef FX9860G
#else
    file_.write((const char*)data, mySize);

    // done ?
    done = file_.good();
#endif // #ifdef DEST_CASIO_CALC

    return done;
}

// read() : Read data from the current file
//
// @data : Pointer to the destination buffer
// @lg : Size in byte to read
// @whence : position (if > 0)
//
// @return : # bytes read
//
int bFile::read(void *data, int lg, int whence){

    if (!data || !lg){
        error_ = BFILE_ERROR_INVALID_PARAMETERS;
        return 0;
    }

    if (!isOpen()){
        error_ = BFILE_ERROR_FILE_NOT_OPENED;
        return 0;
    }


#ifdef DEST_CASIO_CALC
    int read = gint_world_switch(GINT_CALL(BFile_Read, fd_, data,
                                lg, whence));
    if (read < 0){
        error_ = read;  // Error while reading
        return 0;
    }

    error_ = BFILE_NO_ERROR;
    return read;    // #bytes read
#else
    file_.read((char*)data, lg);
    int red(file_.gcount());

    // # bytes read
    return red;
#endif // #ifdef DEST_CASIO_CALC
}

// rename() : Rename or move a file
//
//  @oldPath : name of the file to rename
//  @newPath : destination path
//
//  @return : file successfully renamed ?
//
bool bFile::rename(FONTCHARACTER const oldPath, FONTCHARACTER const newPath){
    // File shouldn't be open
    if (!isOpen()){
#ifdef DEST_CASIO_CALC
#ifdef FX9860G
        if (exist(oldPath)){
            if (open(oldPath, BFile_ReadOnly){
                int size = size();
                bool done(false);
                if (size){
                    // Create buffer
                    char* buffer = (char*)malloc(size);
                    if (buffer &&  read(buffer, size, 0)){
                        // Create a new file with oldPath content
                        bFile newFile();
                        if (newFile.createEx(newPath, BFile_File,
                                &size, BFile_WriteOnly)){
                            newFile.write(buffer, size);
                            newFile.close();
                        }

                        free(buffer);
                        done = true;
                    }
                }

                close();
                if (done){
                    remove(oldPath);
                    error_ = BFILE_NO_ERROR;
                }
                else{
                    error_ = BFILE_ERROR_INVALID_PARAMETERS;
                }

                return done;
            } // if open
        } // if exist

        error_ = BFILE_ERROR_INVALID_FILENAME;
        return false;
#else
        error_ = gint_world_switch(GINT_CALL(BFile_Rename,
                                        oldPath, newPath));
        return (BFILE_NO_ERROR == error_);
#endif // #ifdef FX9860G
#else
    return (0 == std::rename(oldPath, newPath));
#endif // #ifdef DEST_CASIO_CALC
    }

    error_ = BFILE_ERROR_FILE_OPENED;
    return false;
}

// remove() : Remove a file
//
// @filename : name of the file to remove
//
// @return : file successfully removed ?
//
bool bFile::remove(FONTCHARACTER const filename){
    // File shouldn't be open
    if (!isOpen()){
#ifdef DEST_CASIO_CALC
        error_ = gint_world_switch(GINT_CALL(BFile_Remove, filename));
        return (BFILE_NO_ERROR == error_);
#else
    return (0 == std::remove(filename));
#endif // #ifdef DEST_CASIO_CALC
    }

    // File already opened
    error_ = BFILE_ERROR_FILE_OPENED;
    return false;
}

// close() : Close the file
//
void bFile::close(){
#ifdef DEST_CASIO_CALC
    if (fd_){
        error_ = gint_world_switch(GINT_CALL(BFile_Close, fd_));
        fd_ = 0;
        return;
    }
#else
    if (file_.is_open()){
        file_.close();
        error_ = BFILE_NO_ERROR;
    }

    fileName_ = "";
#endif // #ifdef DEST_CASIO_CALC

    error_ = BFILE_ERROR_FILE_NOT_OPENED;
}

// findFirst(): Search the storage memory for paths matching a pattern
//
//  @pattern    FONTCHARACTER glob pattern
//  @sHandle    Will receive search handle
//  @foundFile  Will receive FONTCHARACTER path of matching entry
//  @fileInfo   Will receive metadata of matching entry
//
//  @return :  True on success
//
bool bFile::findFirst(FONTCHARACTER const pattern, SEARCHHANDLE *sHandle,
        FONTCHARACTER foundFile, struct BFile_FileInfo *fileInfo){
    if (sHandle && fileInfo){
#ifdef DEST_CASIO_CALC
        error_ = gint_world_switch(GINT_CALL(BFile_FindFirst,
                pattern, sHandle, foundFile, fileInfo));
        return (BFILE_NO_ERROR == error_);
#else
        (*sHandle) = opendir(pattern);
        if (NULL != (*sHandle)){
            // Read first value
            return findNext(*sHandle, foundFile, fileInfo);
        }
#endif // DEST_CASIO_CALC
    }

    // Error
    error_ = BFILE_ERROR_INVALID_PARAMETERS;
    return false;
}

// findNext(): Continue a search
//
// Continues the search for matches.
//
//  @sHandle : search handle
//  @foundFile : next filename
//  @fileinfo : file metadatas
//
//  @return ;  true if ok
//
bool bFile::findNext(SEARCHHANDLE sHandle, FONTCHARACTER foundFile,
        struct BFile_FileInfo *fileInfo){
    if (sHandle){
#ifdef DEST_CASIO_CALC
        error_ = gint_world_switch(GINT_CALL(BFile_FindNext,
                            sHandle, foundFile, fileInfo));
        return (BFILE_NO_ERROR == error_);
#else
        struct dirent *de;
        if (NULL != (de = readdir(sHandle))){
            fileInfo->type = (DT_DIR == de->d_type?
                BFile_Type_Directory:BFile_Type_File);
                FC_str2FC(de->d_name, foundFile);
            return true;
        }
#endif // #ifdef DEST_CASIO_CALC
    }

    // Invalid handle
    error_ = BFILE_ERROR_INVALID_PARAMETERS;
    return false;
}

// findClose() :  Close a search handle
//
//  @sHandle : Search handle
//
//  @return : done ?
//
bool bFile::findClose(SEARCHHANDLE sHandle){
    if (sHandle){
#ifdef DEST_CASIO_CALC
        error_ = gint_world_switch(GINT_CALL(BFile_FindClose, sHandle));
        return (BFILE_NO_ERROR == error_);
#else
        closedir(sHandle);
        return true;
#endif // #ifdef DEST_CASIO_CALC
    }

    // Invalid handle
    error_ = BFILE_ERROR_INVALID_PARAMETERS;
    return false;
}

//
// Utilities
//

// FC_str2FC() : Convert a string to FC format
//
//  @src : string to convert
//  @dest : destination buffer
//
//  @return : pointer to a FONTCHARACTER
//
bool bFile::FC_str2FC(char* const src, FONTCHARACTER dest){
    size_t len;
    if (!src || !dest || 0 == (len = strlen(src))){
        return false;
    }
#ifdef DEST_CASIO_CALC
   // Copy string content
    char* buffer = (char*)dest;
    for (size_t index = 0; index <= len; index++){
        buffer[2*index] = 0;
        buffer[2*index + 1] = src[index];
    }

    return true;
#else
    return (NULL != strcpy(dest, src));
#endif // #ifdef DEST_CASIO_CALC
}

// FC_FC2str() : Convert a string from FC format to char*
//
//  @src : FC to convert
//  @dest : destination buffer
//
//  @return : pointer to a FONTCHARACTER
//
bool bFile::FC_FC2str(FONTCHARACTER const src, char* dest){
#ifdef DEST_CASIO_CALC
    size_t len;
    if (!src || !dest || 0 == (len = FC_len(src))){
        return false;
    }

   // Copy string content
    char* buffer = (char*)src;
    for (size_t index = 0; index <= len; index++){
        dest[index] = buffer[2*index + 1];
    }

    return true;
#endif // #ifdef DEST_CASIO_CALC
    return false;
}

// FC_cpy() : Copy a FONTCHARACTER to another FONTCHARACTER
//
//  @dest : pointer to a destination string (already allocated)
//  @src : pointer to the source string
//
//  @return : pointer to the copy if done  NULL on error
//
FONTCHARACTER bFile::FC_cpy(FONTCHARACTER dest, FONTCHARACTER const src){
#ifdef DEST_CASIO_CALC
    size_t len(FC_len(src));
    if (0 == len){
        return NULL;
    }

    // Allocates
    int count = (len + 1) * sizeof(uint16_t);
    memcpy((void*)dest, src, count);
    return dest;
#else
    return strcpy(dest, src);
#endif // #ifdef DEST_CASIO_CALC
}

// FC_cat() : Cancatenate 2 FONTCHARACTER
//
//  @dest : pointer to the destination string
//  @add : pointer to the FONTCHARACTER to cancatenate
//
//  @return : pointer to the destination string if done  NULL on error
//
FONTCHARACTER bFile::FC_cat(FONTCHARACTER dest, FONTCHARACTER const add){
    if (!dest || !add){
        return NULL;
    }

#ifdef DEST_CASIO_CALC
    return FC_cpy(dest + FC_len(dest), add);
#else
    return strcat(dest, add);
#endif // DEST_CASIO_CALC
}

// FC_dup() : Duplicate a FONTCHARACTER
//
//  @fName : filename to duplicate
//
//  @return : filename copy or NULL on error
//
FONTCHARACTER bFile::FC_dup(FONTCHARACTER const src){
#ifdef DEST_CASIO_CALC
    size_t len(FC_len(src));
    if (0 == len){
        return NULL;
    }

    // Allocates
    int count = (len + 1) * sizeof(uint16_t);
    FONTCHARACTER dst = (FONTCHARACTER)malloc(count);
    if (dst){
        memcpy((void*)dst, src, count);
    }

    return dst;
#else
    return strdup(src);
#endif // #ifdef DEST_CASIO_CALC
}

// FC_len() : length of a fileName in "char"
//
//  @fName : pointer to the FONTCHARACTER
//
//  @return : size of fName (O on error)
//
size_t bFile::FC_len(FONTCHARACTER const fName){
    if (fName){
#ifdef DEST_CASIO_CALC
        size_t len(0), lChar(sizeof(uint16_t));
        char* buffer = (char*)fName;
        while(buffer[1+ len*lChar]){
            len++;
        }

        return len;
#else
        return strlen(fName);
#endif // #ifdef DEST_CASIO_CALC
    }

    return 0;
}

// EOF
