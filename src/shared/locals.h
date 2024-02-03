//----------------------------------------------------------------------
//--
//--    locals.h
//--
//--        Specific local constants
//--
//----------------------------------------------------------------------

#ifndef __S_SOLVER_LOCAL_CONSTS_h__
#define __S_SOLVER_LOCAL_CONSTS_h__    1

#ifndef DEST_CASIO_CALC

// Folder containing grids
#define CHAR_PATH_SEPARATOR         '/'
#define PATH_SEPARATOR              "/"
#define GRIDS_FOLDER                "/home/jhb/Nextcloud/personnel/JHB/dev/cpp/sudoSolv/grids"

#define KEY_F1      'a'
#define KEY_F6      'y'
#define KEY_EXIT    'x'

enum GAME_KEY{
    KEY_CODE_F1 = 'A',
    KEY_CODE_F6 = 'F',
    KEY_CODE_LEFT = 's',
    KEY_CODE_RIGHT = 'f',
    KEY_CODE_UP = 'e',
    KEY_CODE_DOWN = 'x',
    KEY_CODE_0 = '0',
    KEY_CODE_1 = '1',
    KEY_CODE_2 = '2',
    KEY_CODE_3 = '3',
    KEY_CODE_4 = '4',
    KEY_CODE_5 = '5',
    KEY_CODE_6 = '6',
    KEY_CODE_7 = '7',
    KEY_CODE_8 = '8',
    KEY_CODE_9 = '9',
    KEY_CODE_EXIT = 'q',
    KEY_CODE_EXE = '\13'
};

#define HYP_COLOUR_NONE     ' '
#define HYP_COLOUR_YELLOW   'j'
#define HYP_COLOUR_BLUE     'b'
#define  HYP_COLOUR_GREEN   'v'
#define HYP_COLOUR_RED      'r'

#define STR_RESERVED_BACK       "^ back ^"

#endif // #ifndef DEST_CASIO_CALC
#endif // __S_SOLVER_LOCAL_CONSTS_h__

// EOF
