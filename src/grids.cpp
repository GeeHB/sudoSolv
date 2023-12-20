
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

// Construction
//
grids::grids(const FONTCHARACTER folder){
    index_ = -1;    // No file is selected
    firstFreeID_ = 0;   // Folder is empty
    _browse();
}

// Destruction
//
grids::~grids(){
    // Free list of files
    grids::PFNAME pFile;
    for (size_t index = 0; index < files_.size(); index++){
        if (NULL != (pFile = files_[index])){
            if (pFile->fileName){
                free((void*)pFile->fileName);  // Free the name
            }
            free(pFile);    // free the struct.
        }
    }

    // list is empty
    files_.clear();
}

// nextFile() : Get next file name
//
//  @fName : fullname of the next file
//
//  @return : true if the next file name is valid
//
bool grids::nextFile(FONTCHARACTER& fName){
    index_++;
    if (index_ >= (int)files_.size()){
        return false;
    }

    // Copy the file name
    fName = files_[index_]->fileName;
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

// Internal methods

void _browse
*/

// __atoi()- Convert a filename (without folder) to int
//
//  @src : Filename to convert
//
//  @return : numeric value or -1 on error
//
int grids::__atoi(FONTCHARACTER src){
    char* buffer = (char*)src;
    int num(0);
    uint8_t index(0);
    char car;
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
#ifdef DEST_CASIO_CALC
        index+=2;
#else
        index++;
#endif // DEST_CASIO_CALC

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

    // Append num. value
	int sum ((num < 0)?-1*num:num);
	uint8_t i(0), digit, dCount(0);
	do{
		digit = sum % 10;
		strVal[i++] = '0' + digit;
		if (!(++dCount % 3)){
		    strVal[i++] = ' ';  // for large numbers lisibility
		}

		sum /= 10;
	}while (sum);

	strVal[i] = '\0';

	// Reverse the string (just the num. part)
	__strrev(strVal);

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
