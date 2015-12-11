#ifndef _ui_h
#define _ui_h

#include "genlib.h"
#include "vector.h"
#include "user.h"


int readCommand(String command);

void getInput(User *pUserInput);

Bool checkSaveChanges(User *pUserInput, Command com);

int commandSelection(User *pUserInput, Vector *pContainer);

#endif