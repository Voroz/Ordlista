#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <windows.h>
#include <mscoree.h>
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
#include "errorhand.h"

int main()
{
	system("MODE CON: COLS=113 LINES=32"); //MODE CON[:] [COLS=c] [LINES=n]
	COORD buffer = { 113, 1000 };
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), buffer);

	if (!setlocale(LC_ALL, "")) {
		printf("error while setting locale\n");
	}

	Vector container;
	vectorInit(&container);
	User userInput;
	userNewUser(&userInput);

	printf("WordMagic ver 1.0\nTo get started, type help.");

	int check = 1;
	while (check){
		jmp_buf envMain;
		userSetEnv(setjmp(envMain), &envMain);
		if (userGetNoError()){
			getInput(&userInput);
			check = commandSelection(&userInput, &container);
		}
	}
	return 0;
}
