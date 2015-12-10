#ifndef _ui_h
#define _ui_h

#include "genlib.h"
#include "vector.h"

#define MAX_WORD_LENGTH 100

typedef enum commands{
	help = 1,
	add,
	edit,
	deelete,
	find,
	print,
	load,
	save,
	clear,
	exitProg
} Command;


//
//int commandSelection(User *pUserInput, Vector *pContainer);
//
//int readCommand(String command);
//
//void getInput(User *pUserInput);
//
//Bool checkSaveChanges(User *pUserInput, Command com);

#endif