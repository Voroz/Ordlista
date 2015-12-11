#ifndef _user_h
#define _user_h

#include <stdio.h>
#include <setjmp.h>

#include "genlib.h"
#include "messer.h"
#include "vector.h"

typedef enum command{
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

typedef enum inputType{
	string,
	digit
} InputType;

typedef struct global{
	String filename;
	Bool modified;
} GlobalState;

typedef struct user{
	Command command;
	Vector input;
	GlobalState globals;
} User;


static InputType inType;

static int error;


void userNewUser(User *pUserInput);

void userSetCom(Command com, User *pUserInput);

Command userGetCom(User *pUserInput);

Vector* userGetVector(User *pUserInput);

void userAddInput(void* input, int sizeOfElem, User *pUserInput);

void* userGetInput(User *pUserInput, int index);

void userSetFilename(String filename, User *pUserInput);

String userGetFilename(User *pUserInput);

void userSetMod(int n, User *pUserInput);

Bool userGetMod(User *pUserInput);

void userSetEnv(int n, jmp_buf *env);

int userGetNoError();

void userSetInType(InputType t);

Bool userGetInType();

#endif