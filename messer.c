#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "messer.h"


errorSetEnv(jmp_buf *currentEnv){
	env = currentEnv;
}

void userError(ErrorCode err, ...){
	va_list args;
	va_start(args, err);

	switch (err){
		case (general) :
			fprintf(stderr, "\nSomething went wrong");
			break;
		case (fileOpen) :
			vfprintf(stderr, "\nError opening file \"%s\", does it exist?", args);
			longjmp(env, 1);
		case (saveEmpty) :
			fprintf(stderr, "\nCan't save empty list to file");
			longjmp(env, 1);
		case (noWordFound) :
			vfprintf(stderr, "\nCan't find the word \"%s\" that you're looking for", args);
			longjmp(env, 1);
		case (noSearchResult) :
			vfprintf(stderr, "\nNo match searching for \"%s\"", args);
			longjmp(env, 1);
		case (nullWord) :
			fprintf(stderr, "\nCan't function with an empty word\n");
			longjmp(env, 1);
		case (alreadyExist) :
			vfprintf(stderr, "\nThe word \"%s\" already exist in the list", args);
			longjmp(env, 1);
		case (notAllowedChar) :
			vfprintf(stderr, "\nThe word \"%s\" is not valid", args);
			longjmp(env, 1);
		case (outOfBounds) :
			vfprintf(stderr, "\nThe index %d is out of bounds for your current list of size %d", args);
			longjmp(env, 1);
		case (noFilename) :
			fprintf(stderr, "\nNo filename specified");
			longjmp(env, 1);
		case (toLongInput) :
			fprintf(stderr, "\nInput is to long");
			longjmp(env, 1);
		case (emptyList) :
			vfprintf(stderr, "\nThe list is empty", args);
			break;
		case (unsavedChanges) :
			vfprintf(stderr, "\nYou have changes that are not saved \n- Type %s to discard changes \n- Press enter to return to program", args);
			break;
		case (unknownCommand) :
			vfprintf(stderr, "\nUnknown command", args);
			break;
		default:
			break;
	}
	va_end(args);
}

void userSuccess(SuccessCode succes, ...){
	va_list args;
	va_start(args, succes);

	switch (succes){
		case (wordAdded) :
			vfprintf(stdout, "\nThe word \"%s\" has been added", args);
			break;
		case (wordDelete) :
			vfprintf(stdout, "\nThe word \"%s\" has been deleted", args);
			break;
		case (wordEdit) :
			vfprintf(stdout, "\nThe word \"%s\" has been changed to \"%s\"", args);
			break;
		case (loadFile) :
			vfprintf(stdout, "\nLoaded the file \"%s\"", args);
			break;
		case (saveFile) :
			vfprintf(stdout, "\nSaved the list to file \"%s\"", args);
			break;
		case (clearList) :
			vfprintf(stdout, "\nCleared the list", args);
			break;
		case (continueProg) :
			vfprintf(stdout, "\nGoing back to program", args);
			break;
		default:
			break;
	}
	va_end(args);
}
