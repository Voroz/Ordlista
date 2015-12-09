#include <setjmp.h>

jmp_buf env;

typedef enum success{
	wordAdded,
	wordDelete,
	wordEdit,
	loadFile,
	saveFile,
	clearList,
	continueProg
}SuccessCode;

typedef enum errors{
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
}ErrorCode;

void userError(ErrorCode err, ...);

void userSuccess(SuccessCode succes, ...);
