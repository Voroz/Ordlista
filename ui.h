#ifndef _ui_h
#define _ui_h

#include "genlib.h"
#include "vector.h"

#define MAX_WORD_LENGTH 100

typedef enum{
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


int commandSelection(User, Vector);

int readCommand(String);

Bool checkSaveChanges(User);

void getInput(User);


#endif