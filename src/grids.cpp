
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

#ifndef DEST_CASIO_CALC
using namespace std;
#endif // DEST_CASIO_CALC

#define VECTOR_INCREMENT        10      // # of items added

#define MAX_FILE_ID             0xFFFF

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

// currentFileName() : Get surrent filename
//
//  @fName : current filename
//
//  @return : true if valid
//
bool grids::currentFileName(FONTCHARACTER fName){
    if (-1 == index_ || 0 == count_ || NULL == fName){
        return false;
    }

    // Copy the file name
    bFile::FC_cpy(fName, files_[index_]->fileName);
    return true;
}

// findByID() : Find a file by its ID
//
//  @UID: File's ID
//
//  @return : index of file in list or -1
//
int grids::findByID(int UID){
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
//  @return : index or -1 if list is empty or on error
//
int grids::setPos(int index){
   if (index>=0 && index < count_){
        index_ = index;
   }

    return index_;
}

// browse() : browse folder and fill list with file names and IDs
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

        index_ = -1;
        return 0;
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
            if (BFile_Type_Archived == fileInfo.type ||
                BFile_Type_File == fileInfo.type ){
                _addFile(fName);
            }
        } while(folder.findNext(shandle, fName, &fileInfo));

        folder.findClose(shandle);
    }

    index_ = -1;
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

// getNewFileName() : Generates a unique file name
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
        fName[0] = BFILE_CHAR_ZERO;
#else
        char fName[BFILE_MAX_PATH + 1];
#endif // DEST_CASIO_CALC

        // Get filename
        bFile::FC_cpy((FONTCHARACTER)fName, files_[index_]->fileName);

        // remove from disk
        bFile current;
        if (current.remove(fName)){
            // remove from list
            int ID(__fileName2i(fName));
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

#ifndef DEST_CASIO_CALC
// content() : display list content
//
void grids::content(){
    cout << endl << "__________________________" << endl;
    cout << "Taille : "<< count_ << endl;
    cout << "Capacité : "<< capacity_ << endl;

    PFNAME fname;
    for (int index = 0; index < count_; index++){
        fname = files_[index];
        cout << "\t" << index << " - ";

        if (fname){
            cout << fname->ID;
        }
        else{
            cout << "<vide>";
        }

        cout << endl;
    }
}
#endif // DEST_CASIO_CALC


// Internal methods
//

// _addFile() - Add file to the list
//
//  @fileName : file to add (with no folder)
//
//  @return : true if added
//
bool grids::_addFile(const FONTCHARACTER fileName){
    PFNAME file((PFNAME)malloc(sizeof(FNAME)));
    if (NULL == file){
        return false;
    }
    memset(file, 0x00, sizeof(FNAME));  // empty the struct.

    // Full name
#ifdef DEST_CASIO_CALC
    uint16_t fName[BFILE_MAX_PATH + 1];
#else
    char fName[BFILE_MAX_PATH + 1];
#endif // DEST_CASIO_CALC
    bFile::FC_str2FC(folder_, fName);   // convert folder
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
    return __vector_add(file);
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

// __vector_add() : append an file item pointer to the list
//
//  The file item is inserted ordered based on the uid member of
//  FNAME struct
//
//  This method automaticaly resizes the list
//
//  @file : pointer to the struct to add to the list
//
//  @return : true if succesfully added
//
bool grids::__vector_add(PFNAME file){
    if (NULL == file){
        return false;   // Nothing to add !
    }

    // Empty list => append
    if (!count_){
        return __vector_append(file);
    }

    if (count_ >= capacity_ && !__vector_resize()){
        return false;
    }

    // Find item's position
    int pos(0);
    while(pos < count_ && file->ID >= files_[pos]->ID ){
        pos++;
    }

    if (pos < count_){
        size_t tail((count_ - pos) * sizeof(PFNAME));
        char* buffer((char*)malloc(tail));
        if (!buffer){
            return false;
        }

        // Leave a space for a new item
        memcpy(buffer, &files_[pos], tail);
        memcpy(&files_[pos + 1], buffer, tail);
        free(buffer);
    }

    // Insert (or append) file
    files_[pos] = file;
    count_++;
    return true;    // done
}

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
        return false;   // Nothing to append !
    }

    if (count_ >= capacity_ && !__vector_resize()){
        return false;
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
            capacity_ = count_ = 0;
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
    if (files_){
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

    int ID(MAX_FILE_ID); // max.file ID
    PFNAME pFile(NULL);
    for (int index(0); index < count_; index++){
        if ((pFile = files_[index]) && pFile->ID <= ID){
                ID = pFile->ID + 1;
        }
    }

    return ((MAX_FILE_ID == ID)?-1:ID);
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
        index++;    // shift first NULL char
#else
        uint8_t sCar(1);
#endif // DEST_CASIO_CALC

        num = 0;
        while (num >= 0
            && (car = buffer[index])
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

    // Value or -1
    return (num>=MAX_FILE_ID?-1:num);
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

    // build the string in reverse order
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
