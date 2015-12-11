#ifndef _ui_h
#define _ui_h

#include "genlib.h"
#include "vector.h"
#include "user.h"

/*
* Function: readCommand
* Usage: Command n = readCommand(command);
* -------------------------------------
* Returns a command if string matches any of the commands.
*/
int readCommand(String command);

/*
* Function: getInput
* Usage: getInput(pUserInput);
* -------------------------------------
* Gets input from the user, both command and extra inputs.
*/
void getInput(User *pUserInput);

/*
* Function: checkSaveChanges
* Usage: Bool b = checkSaveChanges(pUserInput, com);
* -------------------------------------
* Check if user wants to save changes.
*/
Bool checkSaveChanges(User *pUserInput, Command com);

/*
* Function: commandSelection
* Usage: int n = commandSelection(pUserInput, pContainer);
* -------------------------------------
* Switches on the command read in getInput().
*/
int commandSelection(User *pUserInput, Vector *pContainer);

#endif