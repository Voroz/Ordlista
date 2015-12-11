#ifndef _user_h
#define _user_h

#include <stdio.h>
#include <setjmp.h>

#include "genlib.h"
#include "messer.h"
#include "vector.h"

/*
* Enum: Command
* -------------------
* Holds the different commands the user can do in the application.
*/
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

/*
* Enum: InputType
* -------------------
* Holds the different kinds of inputs the user can do in the application.
*/
typedef enum inputType{
	string,
	digit
} InputType;

/*
* Struct: GlobalState
* -------------------
* Holds some information that the application needs saved.
*/
typedef struct global{
	String filename;
	Bool modified;
} GlobalState;

/*
* Struct: User
* -------------------
* Stores the current entered command from the user and extra inputs.
*/
typedef struct user{
	Command command;
	Vector input;
	GlobalState globals;
} User;

/*
* Static: inType
* -------------------
* Stores the current input type from the user.
*/
static InputType inType;

/*
* Static: error
* -------------------
* Stores the status if their is an error. Zero if there is no error and one if an error occurred.
*/
static int error;

/*
* Function: userNewUser
* Usage: userNewUser(pUserInput);
* -------------------------------------
* Creates a new user.
*/
void userNewUser(User *pUserInput);

/*
* Function: userSetCom
* Usage: userSetCom(com, pUserInput);
* -------------------------------------
* Sets the command in the User struct.
*/
void userSetCom(Command com, User *pUserInput);

/*
* Function: userGetCom
* Usage: Command c = userGetCom(pUserInput);
* -------------------------------------
* Gets the current command from the User struct.
*/
Command userGetCom(User *pUserInput);

/*
* Function: userGetVector
* Usage: Vector v = userGetVector(pUserInput);
* -------------------------------------
* Gets the input vector from the User struct.
*/
Vector* userGetVector(User *pUserInput);

/*
* Function: userAddInput
* Usage: userAddInput(input, sizeOfElem, pUserInput);
* -------------------------------------
* Appends a new input in the User struct.
*/
void userAddInput(void* input, int sizeOfElem, User *pUserInput);

/*
* Function: userGetInput
* Usage: userGetInput(pUserInput, index);
* -------------------------------------
* Gets the input vector from the User struct.
*/
void* userGetInput(User *pUserInput, int index);

/*
* Function: userSetFilename
* Usage: userSetFilename(filename, pUserInput);
* -------------------------------------
* Sets the current filename to save in the User struct.
*/
void userSetFilename(String filename, User *pUserInput);

/*
* Function: userGetFilename
* Usage: String s = userGetFilename(pUserInput);
* -------------------------------------
* Gets the filename stored in the User struct.
*/
String userGetFilename(User *pUserInput);

/*
* Function: userSetMod
* Usage: userSetMod(n, pUserInput);
* -------------------------------------
* Set status of modified varible.
*/
void userSetMod(int n, User *pUserInput);

/*
* Function: userGetMod
* Usage: Bool b = userGetMod(pUserInput);
* -------------------------------------
* Gets the modified variable from the User struct.
*/
Bool userGetMod(User *pUserInput);

/*
* Function: userSetMod
* Usage: userSetMod(n, env);
* -------------------------------------
* Sets the static error variable and injects a pointer
* to the current enviroment to error handling code in "messer.h".
*/
void userSetEnv(int n, jmp_buf *env);

/*
* Function: userGetNoError
* Usage: int n = userGetNoError();
* -------------------------------------
* Used after 'userSetEnv' function to determine if an error occurred.
*/
int userGetNoError();

/*
* Function: userSetInType
* Usage: userSetInType(t);
* -------------------------------------
* Sets if the current input from user if it's a number or string.
*/
void userSetInType(InputType t);

/*
* Function: userGetInType
* Usage: Bool b = userGetInType();
* -------------------------------------
* Returns the value set from 'userSetInType' function.
* If input is digit a one is returned else a zero if input is of type string.
*/
Bool userGetInType();

#endif