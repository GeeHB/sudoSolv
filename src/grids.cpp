
//---------------------------------------------------------------------------
//--
//--    grids.cpp
//--
//--        Implementation of grids object - List of grid files stored on "disk"
//--
//--        The object works as a rudimentary DB
//--        where each file has a numeric ID
//--
//---------------------------------------------------------------------------

#include "grids.h"
#include <cstdlib>

#define VECTOR_INCREMENT        10      // # of items addded

// Construction
//
grids::grids(){
    index_ = -1;    // No file is selected

    //firstFreeID_ = 0;   // Folder is empty
    files_ = NULL;
    count_ = 0;
    capacity_ = 0;

    // this folder
    strcpy(folder_, GRIDS_FOLDER);
    strcat(folder_, PATH_SEPARATOR);

    _browse();
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

/*

// File management
bool newFileName(FONTCHARACTER folder);
*/

// deleteFile() : Delete current file
//
//  @return : true if deleted
//
bool grids::deleteFile(){
    if (index_ != -1 && index_ < count_){
#ifdef DEST_CASIO_CALC
        uint16_t fName[BFILE_MAX_PATH + 1];
#else
        char fName[BFILE_MAX_PATH + 1];
#endif // DEST_CASIO_CALC

        // Get filename
        bFile::FC_cpy((FONTCHARACTER)fName, files_[index_]->fileName);

        bFile current;
        return current.remove(fName);
    }

    return false;
}


// Internal methods
//

// _browse() : browse folder and fill list with file names
//
void grids::_browse(){

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
            return;
        }
    }

    // Empty the list
    if (capacity_){
        __vector_clear();
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
}

// _addFile() - Add file to the list
//
//  @fileName : file to add
//
//  @return : true if added
//
bool grids::_addFile(FONTCHARACTER fileName){
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
void grids::__vector_clear(){
    PFNAME pFile(NULL);
    for (int index=0; index < count_; index++){
        if ((pFile = files_[index])){
            if (pFile->fileName){
                free((void*)pFile->fileName);  // Free the name
            }

            free(pFile);    // Free the struct.
        }
    }

    // The list is empty
    free(files_);
    capacity_ = 0;
    count_ = 0;
    files_ = NULL;
}

//
// Utils
//

// __fileName2i()- Convert a fully qualified filename to int
//
//  @src : Filename to convert
//
//  @return : numeric value or -1 on error
//
int grids::__fileName2i(FONTCHARACTER src){
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
            && car != '.')      // no extension
        {
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
