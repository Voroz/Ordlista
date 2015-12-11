#ifndef _strmanip_h
#define _strmanip_h

#include "genlib.h"

/*
* Function: convertToSweChar
* Usage: char c = convertToSweChar(input);
* -------------------------------------
* Converts the input if it's an å,ä,ö or Å,Ä,Ö to a different ascii char.
*/
char convertToSweChar(int input);

/*
* Function: convertToSweString
* Usage: convertToSweString(input);
* -------------------------------------
* Converts every character in input if it contins å,ä,ö or Å,Ä,Ö to a different ascii char.
*/
void convertToSweString(String input);

/*
* Function: stringIsEmpty
* Usage: Bool b = stringIsEmpty(s);
* -------------------------------------
* Checks if string is of lenght 0.
*/
Bool stringIsEmpty(String s);

/*
* Function: stringIsNumber
* Usage: Bool b = stringIsNumber(s);
* -------------------------------------
* Ckeck if every character is a digit.
*/
Bool stringIsNumber(String s);

/*
* Function: stringIsAlpha
* Usage: Bool b = stringIsAlpha(s);
* -------------------------------------
* Check if every character is a letter.
*/
Bool stringIsAlpha(String s);

/*
* Function: stringFormat
* Usage: stringFormat(s);
* -------------------------------------
* Formats the input string to a specific format used in this spplication.
*/
void stringFormat(String s);

/*
* Function: compareSwedish
* Usage: compareSwedish(s);
* -------------------------------------
* Shifts 'Ä' and 'Å' position in the input string,
* so they can be sorted in correct order according to swedish alphabet.
*/
void compareSwedish(String s);

#endif