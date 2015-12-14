#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <windows.h>

#include "genlib.h"
#include "random.h"
#include "simpio.h"
#include "strlib.h"
#include "fileio.h"
#include "listfunc.h"
#include "strmanip.h"
#include "ui.h"
#include "user.h"
#include "vector.h"
#include "messer.h"

#define WINDOW_WIDTH "113"
#define WINDOW_HEIGHT "32"

int main(){
	// Set console size and buffer
	system("MODE CON: COLS="WINDOW_WIDTH" LINES="WINDOW_HEIGHT);
	COORD buffer = { 113, 1000 };
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), buffer);

	// Set locale settings to what is selected in the environment
	if (!setlocale(LC_ALL, "")){
		printf("error while setting locale\n");
	}

	// Create vector to contain all the words
	Vector container;
	vectorInit(&container);
	// Create new user to handle input
	User userInput;
	userNewUser(&userInput);

	printf("WordMagic ver 1.0\nTo get started, type help.");

	int app = 1;
	while (app){
		// Set return point if an error occurs
		jmp_buf envMain;
		userSetEnv(setjmp(envMain), &envMain);
		if (userGetNoError()){
			getInput(&userInput); // Read input from user
			app = commandSelection(&userInput, &container); // Execute user command
		}
	}
	return 0;
}
