
//----------------------------------------------------------------------
//--
//--    grids.cpp
//--
//--        Implementation of grids object - List of grid files in a folder
//--
//--        The object works as a rudimentary DB
//--        where each file has a numeric ID
//--
//----------------------------------------------------------------------

#include "grids.h"
#include <cstdlib>

#define VECTOR_INCREMENT        10      // # of items addded

// Construction
//
grids::grids(){
    index_ = -1;    // No file is selected
    files_ = NULL;  // Folder is empty
    count_ = 0;
    capacity_ = 0;

    // this folder
    strcpy(folder_, GRIDS_FOLDER);
    strcat(folder_, PATH_SEPARATOR);
}

// Destruction
//
grids::~grids(){
    __vector_clear(true);
}

// findPos() : Find the index (position) of a file in the internal list
//
//  @UID: File's ID
//
//  @return : index of file in liste or -1
//
int grids::findPos(int UID){
    PFNAME pFile(NULL);
    for (int index(0); index < count_; index++){
        if ((pFile = files_[index]) && pFile->ID == UID){
            return index;
        }
    }

    // Not found
    return -1;
}

// setPos() : Set current position index in list
//
//  @index: new position index
//
//  @return : index or -1 if list is empty
//
int grids::setPos(int index){
   if (index>=0 && index < count_){
        index_ = index;
   }

    return index_;
}

// browse() : browse folder and fill list with file names
//
//  @return : count of elements read or -1 if error
//
int grids::browse(){
    bFile folder;
    char szPattern[BFILE_MAX_PATH + 1];

#ifdef DEST_CASIO_CALC
    uint16_t fName[BFILE_MAX_PATH + 1];
    uint16_t FCPattern[BFILE_MAX_PATH + 1];
#else
    char fName[BFILE_MAX_PATH + 1];
    char FCPattern[BFILE_MAX_PATH + 1];
#endif // DEST_CASIO_CALC

    // Ensure folder exist
    strcpy(szPattern, GRIDS_FOLDER);
    folder.FC_str2FC(szPattern, fName);
    if (!folder.exist(fName)){
        // create the folder
        if (!folder.create(fName, BFile_Folder, NULL)){
            return -1;
        }
    }

    // Empty the list
    if (capacity_){
        __vector_clear(false);
    }

    // Browse folder
    SEARCHHANDLE shandle;
    struct BFile_FileInfo fileInfo;

#ifdef DEST_CASIO_CALC
    strcat(szPattern, PATH_SEPARATOR);
    strcat(szPattern, GRID_FILE_SEARCH_PATTERN);
#endif // DEST_CASIO_PATTERN
    folder.FC_str2FC(szPattern, FCPattern);

    if (folder.findFirst(FCPattern, &shandle, fName, &fileInfo)){
        do{
            // a file ?
            if (BFile_Type_Archived == fileInfo.type){
                _addFile(fName);
            }
        } while(folder.findNext(shandle, fName, &fileInfo));

        folder.findClose(shandle);
    }

    return count_;
}

// addFileName() : Add a filename to the folder's content list
//
//  @fName : filename to add
//
//  @return : true if successfully added
//
bool grids::addFileName(const FONTCHARACTER fName){
    size_t len(0);
    if (!(len = bFile::FC_len(fName))){
        return false;
    }

    // If filename is a FQN, remove path ...
    char* buffer = (char*)fName;
    while (len && CHAR_PATH_SEPARATOR != buffer[2*len-1]){
        len--;
    }

    // Add file to the list
    return _addFile((len?(fName+len+1):fName));
}

// nextFile() : Get next file name
//
//  @fName : fullname of the next file
//
//  @return : true if the next file name is valid
//
bool grids::nextFile(FONTCHARACTER fName){
    if (index_ >= count_){
        return false;
    }

    // Copy the file name
    bFile::FC_cpy(fName, files_[++index_]->fileName);
    return true;
}

// prevFile() : Get previous file name
//
//  @fName : fullname of the previous file
//
//  @return : true if the previous file name is valid
//
bool grids::prevFile(FONTCHARACTER fName){
    if (index_ <= 0){
        return false;
    }

    // Copy the file name
    bFile::FC_cpy(fName, files_[--index_]->fileName);
    return true;
}

// newFileName() : Generates a unique file name
//
//  @fName : new FQN
//  @UID : pointer to int who will receive file's unique ID
//          can be NULL
//
//  @return : true if name is valid
//
bool grids::getNewFileName(FONTCHARACTER fName, int* UID){
    if (fName){
        int ID(_nextFileID());
        if (-1 != ID){
            char name[BFILE_MAX_PATH + 1];
            strcpy(name, folder_);
            __itoa(ID, name + strlen(name));    // Append ID
            strcat(name, GRID_FILE_EXT);

            bFile::FC_str2FC(name, fName);
            if (UID){
                (*UID) = ID;
            }
            return true;
        }
    }

    // No filename or no possible ID
    return false;
}

// deleteFile() : Delete current file
//
//  @return : true if deleted
//
bool grids::deleteFile(){
    if (index_ != -1 && index_ < count_){
#ifdef DEST_CASIO_CALC
        uint16_t fName[BFILE_MAX_PATH + 1];
        fName[0] = BFILE_CHAR_NULL;
#else
        char fName[BFILE_MAX_PATH + 1];
#endif // DEST_CASIO_CALC

        // Get filename
        bFile::FC_cpy((FONTCHARACTER)fName, files_[index_]->fileName);

        // remove from disk
        bFile current;
        if (current.remove(fName)){
            // remove from list
            int ID = __fileName2i(fName);
            PFNAME pFile(NULL);
            bool done(false);
            for (int index=0; !done && index < count_; index++){
                if ((pFile = files_[index]) && ID == pFile->ID){
                    _freeFileName(pFile);
                    files_[index] = NULL;
                    done = true;
                }
            }

            return true;
        }
    }

    return false;
}


// Internal methods
//

// _addFile() - Add file to the list
//
//  @fileName : file to add (with no folder)
//
//  @return : true if added
//
bool grids::_addFile(const FONTCHARACTER fileName){
    PFNAME file = (PFNAME)malloc(sizeof(FNAME));
    if (NULL == file){
        return false;
    }
    memset(file, 0x00, sizeof(FNAME));  // empty struct.

    // Full name
#ifdef DEST_CASIO_CALC
    uint16_t fName[BFILE_MAX_PATH + 1];
#else
    char fName[BFILE_MAX_PATH + 1];
#endif // DEST_CASIO_CALC
    bFile::FC_str2FC(folder_, fName);   // convert folder to FONTCHARACTER
    bFile::FC_cat(fName, fileName);     // add filename

    // fill struct. with file informations
    if (NULL == (file->fileName = bFile::FC_dup(fName)) ||
        -1 == (file->ID = __fileName2i(fileName))){
        if (file->fileName){
            free((void*)file->fileName);
        }

        free(file);
        return false;   // Unable to copy the filename or invalid name
    }

    // Add
    __vector_append(file);
    return true;
}

// _freeFileName : free the menory used by the FNAME struct.
//
//  @pItem : pointer to item to be freed
//
void grids::_freeFileName(PFNAME pItem){
    if (pItem){
        if (pItem->fileName){
                free((void*)pItem->fileName);  // Free the name
            }

        free(pItem);    // Free the struct.
    }
}

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
bool grids::__vector_append(PFNAME file){
    if (NULL == file){
        return false;   // Nothing to add !
    }

    if (count_ >= capacity_){
        if (!__vector_resize()){
            return false;
        }
    }

    // Append to the list
    files_[count_++] = file;
    return true;
}

// __vector_resize() : resize the list
//
//  @return : true if succesfully resized
//
bool grids::__vector_resize(){
    size_t cBytes;

    // First allocation ?
    if (NULL == files_){
        capacity_ = VECTOR_INCREMENT;
        count_ = 0;

        cBytes = capacity_ * sizeof(PFNAME);
        if (NULL == (files_ = (PFNAME*)malloc(cBytes))){
            capacity_ = 0;
            return false;
        }

        // list is empty
        memset(files_, 0x00, cBytes);
        return true;
    }

    // resize the "vector"
    if (count_ >= capacity_) {
        // New size
        capacity_ += VECTOR_INCREMENT;
        files_ = (PFNAME*)realloc(files_, capacity_ * sizeof(PFNAME));
        if (NULL == files_) {
            return false;
        }
    }

    // done
    return true;
}

// __vector_clear() : clear the list and its content
//
//  @freeList : free list as well ?
//
void grids::__vector_clear(bool freeList){
    PFNAME pFile(NULL);
    for (int index(0); index < count_; index++){
        if ((pFile = files_[index])){
            _freeFileName(pFile);
        }
    }

    count_ = 0; // List is empty

    if (freeList){
        free(files_);
        capacity_ = 0;
        files_ = NULL;
    }
}

//
// Utils
//

// _nextFileID()- Get next ID for a new file
//
//  @return : numeric value or -1 on error
//
int grids::_nextFileID(){
    if (!count_){
        return 0;   // Empty folder
    }

    int ID(0xFFFF); // max.file ID
    PFNAME pFile(NULL);
    for (int index(0); index < count_; index++){
        if ((pFile = files_[index]) && pFile->ID <= ID){
                ID = pFile->ID + 1;
        }
    }

    return ((0xFFFF == ID)?-1:ID);
}

// __fileName2i()- Convert a fully qualified filename to int
//
//  @src : Filename to convert
//
//  @return : numeric value or -1 on error
//
int grids::__fileName2i(const FONTCHARACTER src){
    int num(-1);
    if (src){
        char* buffer = (char*)src;
        uint8_t index(0);
        char car;
#ifdef DEST_CASIO_CALC
        uint8_t sCar(sizeof(uint16_t));
#else
        uint8_t sCar(1);
#endif // DEST_CASIO_CALC

        num = 0;
        while (num >= 0
            && (car = buffer[index + 1])
            && car != '.'){      // no extension
            if (car >= '0' && car <= '9'){
                num = num * 10 + (car - '0');
            }
            else{
                num = -1;   // invalid char
            }

            // Next char.
            index+=sCar;
        }
    }

    // Done or error
    return num;
}

// __itoa() : Transform a numeric value into a string
//
//  @num : Numeric value to transform
//  @str : Pointer to output string
//
//  @return : pointer to formated string
//
char* grids::__itoa(int num, char* str){
    char* strVal(str);

    int sum ((num < 0)?-1*num:num);
	uint8_t i(0), digit;

	// buid the string in reverse order
	do{
		digit = sum % 10;
		strVal[i++] = '0' + digit;
		sum /= 10;
	}while (sum);

	strVal[i] = '\0';
	__strrev(strVal);   // reverse the string
	return str;
}

// __strrev() : Reverse a string
//
//  @str : String to reverse
//
void grids::__strrev(char *str){
	int i, j;
	unsigned char a;
	size_t len = strlen((const char *)str);
	for (i = 0, j = len - 1; i < j; i++, j--){
		a = str[i];
		str[i] = str[j];
		str[j] = a;
	}
}

// EOF
