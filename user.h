#ifndef _user_h
#define _user_h

#include <stdio.h>
#include "errorhand.h"

typedef enum{
	string,
	digit
} InputType;

typedef struct{
	String filename;
	Bool modified;
} GlobalState;

typedef struct{
	Command command;
	Vector input;
	GlobalState globals;
} User;



static InputType inType;
static int error;



void userNewUser(User *pUserInput);

void userSetCom(Command com, User *pUserInput);

Command userGetCom(User *pUserInput);

void userAddInput(void* input, int sizeOfElem, User *pUserInput);

Vector* userGetInput(User *pUserInput);

void userSetFilename(String filename, User *pUserInput);

String userGetFilename(User *pUserInput);

void userSetMod(int n, User *pUserInput);

Bool userGetMod(User *pUserInput);

void userSetEnv(int n, jmp_buf *env);

int userGetNoError();

void userSetInType(InputType t);

Bool userGetInType();

#endif