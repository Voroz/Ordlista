#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <stdio.h>

#include "ui.h"
#include "user.h"
#include "messer.h"
#include "listfunc.h"
#include "strmanip.h"
#include "simpio.h"
#include "strlib.h"

#define MAX_INPUT_LENGTH 100

int readCommand(String command){
	if (StringEqual("help", command)){
		return help;
	}
	if (StringEqual("add", command)){
		return add;
	}
	if (StringEqual("delete", command)){
		return deelete;
	}
	if (StringEqual("find", command)){
		return find;
	}
	if (StringEqual("print", command)){
		return print;
	}
	if (StringEqual("load", command)){
		return load;
	}
	if (StringEqual("save", command)){
		return save;
	}
	if (StringEqual("clear", command)){
		return clear;
	}
	if (StringEqual("edit", command)){
		return edit;
	}
	if (StringEqual("exit", command)){
		return exitProg;
	}
	return 0;
}

void getInput(User *pUserInput){
	printf("\n\n%c", '>');

	String	userInputRaw,
			command = GetBlock(MAX_INPUT_LENGTH),
			input1 = GetBlock(MAX_INPUT_LENGTH),
			input2 = GetBlock(MAX_INPUT_LENGTH);

	int *inpuyArray[2] = { &input1, &input2 };
	Command com;

	userInputRaw = GetLine(); // Get input string

	if (StringLength(userInputRaw) > MAX_INPUT_LENGTH){ // Make sure input is not to long
		userError(toLongInput);
	}
	int numberOfInputs = sscanf(userInputRaw, "%s %s %s", command, *inpuyArray[0], *inpuyArray[1]); // Format input string, get amount of inputs

	vectorClear(userGetVector(pUserInput)); // Clear vector of old input
	com = readCommand(command);
	userSetCom(com, pUserInput);

	if (numberOfInputs == 1){
		userSetInType(string);
		userAddInput("", 1, pUserInput);
	}
	else{
		for (int inputs = 0; (inputs < (numberOfInputs - 1)); inputs++){
			// stringFormat(*inpuyArray[inputs]); // Format input to our specified standard format
			if (stringIsNumber(*inpuyArray[inputs]) && (com == deelete || com == edit)){
				userSetInType(digit);
				int number = StringToInteger(*inpuyArray[inputs]); // Make the input string to an int
				userAddInput(number, sizeof(int), pUserInput);
			}
			else{
				userSetInType(string);
				userAddInput(*inpuyArray[inputs], (StringLength(*inpuyArray[inputs]) + 1), pUserInput);
			}
			if (com == load || com == save){ // Save filename when user loads or save
				userSetFilename(*inpuyArray[0], pUserInput); // Always save first input as filename
			}
		}
	}
	
	FreeBlock(userInputRaw);
	FreeBlock(command);
	FreeBlock(input1);
	FreeBlock(input2);
}

Bool checkSaveChanges(User *pUserInput, Command com){
	if (userGetMod(pUserInput)){
		jmp_buf envSaveChanges;
		userSetEnv(setjmp(envSaveChanges), &envSaveChanges);
		if (userGetNoError()){
			userError(unsavedChanges, "exit");
			getInput(pUserInput);
			if (userGetCom(pUserInput) == exitProg){
				return TRUE;
			}
		}
		userSuccess(continueProg);
		return FALSE;
	}
	return TRUE;
}

int commandSelection(User *pUserInput, Vector *pContainer){

	Command command = userGetCom(pUserInput);
	Bool inputIsDigit = userGetInType(pUserInput);
	void **input = userGetInput(pUserInput, 0);

	int userWantsToQuit;

	switch (command){

	case (help) :
		printHelpInfo();
		return 1;

	case (add) :
		addWord(input, findPosForNewWord(input, pContainer), pContainer);
		userSetMod(1, pUserInput);
		return 1;

	case (deelete) :
		if (inputIsDigit){
			deleteWord(*input, pContainer);
		}
		else{
			deleteWord(getPosForWord(input, pContainer), pContainer);
		}
		userSetMod(1, pUserInput);
		return 1;

	case (edit) :
		if (inputIsDigit){
			editWord(*input, pContainer);
		}
		else{
			editWord(getPosForWord(input, pContainer), pContainer);
		}
		userSetMod(1, pUserInput);
		return 1;

	case (find) :
		searchVector(input, pContainer);
		return 1;

	case (print) :
		printWordsInVector(pContainer, 0, vectorSize(pContainer));
		return 1;

	case (load) :
		if (stringIsEmpty(input)){ // User always have to specify which file to load
			userError(noFilename);
		}
		else{
			loadWordsFromFile(userGetFilename(pUserInput), pContainer);
		}
		userSetMod(0, pUserInput);
		return 1;

	case (save) :
		saveWordsToFile(userGetFilename(pUserInput), pContainer);
		userSetMod(0, pUserInput);
		return 1;

	case (clear) :
		vectorClear(pContainer);
		userSuccess(clearList);
		return 1;

	case (exitProg) :
		userWantsToQuit = checkSaveChanges(pUserInput, exitProg);
		if (userWantsToQuit){
			return 0;
		}
		return 1;

	default:
		userError(unknownCommand);
		return 1;
	}
}
