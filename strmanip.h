#ifndef _strmanip_h
#define _strmanip_h

#include "genlib.h"

char convertToSweChar(int input);

void convertToSweString(String input);

Bool stringIsEmpty(String s);

Bool stringEqualNotCaseSens(String wordA, String wordB);

// Ckeck if every character is a digit
Bool stringIsNumber(String s);

// Check if every character is a letter
Bool stringIsAlpha(String s);

void stringFormat(String s);

void compareSwedish(String s);

#endif