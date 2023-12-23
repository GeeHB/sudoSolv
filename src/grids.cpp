
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
grids::grids(const FONTCHARACTER folder){
    index_ = -1;    // No file is selected

    //firstFreeID_ = 0;   // Folder is empty
    files_ = NULL;
    count_ = 0;
    capacity_ = 0;

    _browse();
}

// nextFile() : Get next file name
//
//  @fName : fullname of the next file
//
//  @return : true if the next file name is valid
//
bool grids::nextFile(FONTCHARACTER& fName){
    if (index_ >= (size() - 1)){
        return false;
    }

    // Copy the file name
    fName = files_[++index_]->fileName;
    return true;
}

// prevFile() : Get previous file name
//
//  @fName : fullname of the previous file
//
//  @return : true if the previous file name is valid
//
bool grids::prevFile(FONTCHARACTER& fName){
    if (index_ <= 0){
        return false;
    }

    // Copy the file name
    fName = files_[--index_]->fileName;
    return true;
}

/*

// File management
bool newFileName(FONTCHARACTER folder);

bool deleteFile();  // current
*/


// Internal methods
//

// _browse() : browse folder and fill list with file names
//
void grids::_browse(){

    bFile folder;
    char szPattern[BFILE_MAX_PATH + 1];

#ifdef DEST_CASIO_CALC
    uint16_t fName[255];
    uint16_t FCPattern[255];
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
    int shandle;
    struct BFile_FileInfo fileInfo;

    strcpy(szPattern, GRIDS_FOLDER);
#ifdef DEST_CASIO_CALC
    strcat(szPattern, PATH_SEPARATOR);
    strcat(szPattern, GRID_FILE_SEARCH_PATTERN);
#endif // DEST_CASIO_PATTERN
    folder.FC_str2FC(szPattern, FCPattern);

    if (folder.findFirst(FCPattern, &shandle, fName, &fileInfo)){
        do{
            // a file ?
            if (BFile_Type_File == fileInfo.type){
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
//  @return : ture if added
//
bool grids::_addFile(FONTCHARACTER fileName){
    PFNAME file = (PFNAME)malloc(sizeof(FNAME));
    if (NULL == file){
        return false;
    }

    // fill struct. with file informations
    if (NULL == (file->fileName = bFile::FC_dup(fileName)) ||
        -1 == (file->ID = __atoi(fileName))){
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
        if (NULL == files_){
            capacity_ = 0;
            return false;
        }

        // list is empty
        files_ = (PFNAME*)malloc(cBytes);
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

// __vector_CLEAR() : clear the list and its content
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
// strings utils
//

// __atoi()- Convert a filename (without folder) to int
//
//  @src : Filename to convert
//
//  @return : numeric value or -1 on error
//
int grids::__atoi(FONTCHARACTER src){
    size_t len;
    if (0 == (len = bFile::FC_len(src))){
        return -1;
    }

    char* buffer = (char*)src;
    int num(0);
    uint8_t index(0);
    char car;

    uint8_t sCar(1);
#ifdef DEST_CASIO_CALC
    sCar = 2;
#endif // DEST_CASIO_CALC

    // remove path
    if (len > 1){
        int index = len - sCar;
        while (index && buffer[index] != CHAR_PATH_SEPARATOR){
            index-=sCar;
        }

        buffer=(index?buffer+(index+sCar):buffer);
    }

    // convert the file name
    index = 0;
    while (num >= 0 && buffer[index]){
        car = buffer[index];
        if (car >= '0' && car <= '9'){
            num = num * 10 + (car - '0');
        }
        else{
            // Invalid char.
            num = -1;
        }

        // Next char.
        index+=sCar;
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
