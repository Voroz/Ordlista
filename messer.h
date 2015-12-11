#ifndef _messer_h
#define _messer_h

#include <setjmp.h>

typedef enum succes{
	wordAdded,
	wordDelete,
	wordEdit,
	loadFile,
	saveFile,
	clearList,
	continueProg
} SuccessCode;

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

static jmp_buf *env;

errorSetEnv(jmp_buf *currentEnv);

void userError(ErrorCode err, ...);

void userSuccess(SuccessCode succes, ...);

#endif