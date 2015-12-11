#ifndef _messer_h
#define _messer_h

#include <setjmp.h>

/*
* Enum: SuccessCode
* -------------------
* Holds the different succes codes.
*/
typedef enum succes{
	wordAdded,
	wordDelete,
	wordEdit,
	loadFile,
	saveFile,
	clearList,
	continueProg
} SuccessCode;

/*
* Enum: ErrorCode
* -------------------
* Holds the different error codes.
*/
typedef enum error{
	general,
	fileOpen,
	saveEmpty,
	noWordFound,
	noSearchResult,
	nullWord,
	alreadyExist,
	notAllowedChar,
	outOfBounds,
	noFilename,
	toLongInput,
	emptyList,
	unsavedChanges,
	unknownCommand
} ErrorCode;

/*
* Static: env
* -------------------
* Used to store a pointer to the current enviroment.
*/
static jmp_buf *env;

/*
* Function: errorSetEnv
* Usage: errorSetEnv(currentEnv);
* -------------------------------------
* Set the 'env' to the current enviroment.
*/
void errorSetEnv(jmp_buf *currentEnv);

/*
* Function: userError
* Usage: userError(err, ...);
* -------------------------------------
* Prints an error message to the screen, depending on which error code the function recive.
*/
void userError(ErrorCode err, ...);

/*
* Function: userSuccess
* Usage: userSuccess(succes, ...);
* -------------------------------------
* Prints a message to the screen, depending on which succes code the function recive.
*/
void userSuccess(SuccessCode succes, ...);

#endif