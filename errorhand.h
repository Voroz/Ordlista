#ifndef _errorhand_h
#define _errorhand_h

#include <setjmp.h>

typedef enum{
	wordAdded,
	wordDelete,
	wordEdit,
	loadFile,
	saveFile,
	clearList,
	continueProg
} SuccessCode;

typedef enum{
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