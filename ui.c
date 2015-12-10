#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <stdio.h>
#include "ui.h"
#include "user.h"
#include "errorhand.h"
#include "listfunc.h"
#include "strmanip.h"
#include "simpio.h"
#include "strlib.h"

int commandSelection(User *pUserInput, Vector *pContainer){

	Command command = userGetCom(pUserInput);
	Bool inputIsDigit = userGetInType(pUserInput);
	void **input;
	int userWantsToQuit, userWantsToLoad;

	if (inputIsDigit){
		input = (int*)vectorGet(userGetInput(pUserInput), 0);
	}
	else{
		input = (String)vectorGet(userGetInput(pUserInput), 0);
	}

	switch (command){

	case (help) :
		printHelpInfo();
		return 1;

	case (add) :
		addWord(input, findPosForWord(input, pContainer), pContainer);
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
		loadWordsFromFile(input, pContainer);
		userSetMod(0, pUserInput);
		return 1;

	case (save) :
		if (!stringIsEmpty(userGetFilename(pUserInput))){
			saveWordsToFile(userGetFilename(pUserInput), pContainer); // Save to last active file
		}
		else{
			saveWordsToFile(input, pContainer); // Save to file specified in 'input'
		}
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

// TODO: Use not yet made function to format 'input1', 'input2'
void getInput(User *pUserInput){
	printf("\n\n%c", '>');

	String	userInputRaw,
			command = GetBlock(MAX_WORD_LENGTH),
			input1 = GetBlock(MAX_WORD_LENGTH),
			input2 = GetBlock(MAX_WORD_LENGTH);

	userInputRaw = GetLine(); // Get input string

	if (StringLength(userInputRaw) > MAX_WORD_LENGTH){ // Make sure input is not to long (arbitrary limit)
		userError(toLongInput);
	}
	int numberOfInputs = sscanf(userInputRaw, "%s %s %s", command, input1, input2); // Format input string, get amount of inputs

	vectorClear(userGetInput(pUserInput));
	Command com = readCommand(command);
	userSetCom(com, pUserInput);

	if (numberOfInputs > 1){
		convertToSweString(input1);
		if (stringIsNumber(input1) && com == deelete || com == edit){
			userSetInType(digit);
			int number = StringToInteger(input1); // Make the string 'input' to an int
			userAddInput(number, sizeof(int), pUserInput);
		}
		else{
			userSetInType(string);
			userAddInput(input1, (StringLength(input1) + 1), pUserInput);
			if (numberOfInputs > 2){
				convertToSweString(input2);
				userAddInput(input2, (StringLength(input2) + 1), pUserInput);
			}
		}
		if (com == load || com == save){ // Save filename when user loads or save
			userSetFilename(input1, pUserInput);
		}
	}
	else{
		userSetInType(string);
		userAddInput("", 1, pUserInput); // If no input set 'input1' to empty string to simplify problems down the line
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