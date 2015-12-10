#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "genlib.h"
#include "strlib.h"

char convertToSweChar(int input){
	//å
	if (input == -122){
		return -27;
	}
	//ä
	if (input == -124){
		return -28;
	}
	//ö
	if (input == -108){
		return -10;
	}
	//Å
	if (input == -113){
		return -59;
	}
	//Ä
	if (input == -114){
		return -60;
	}
	//Ö
	if (input == -103){
		return -42;
	}
	return input;
}

void convertToSweString(String input){
	for (int i = 0; i < StringLength(input); i++){
		input[i] = convertToSweChar(input[i]);
	}
}

Bool stringIsEmpty(String s){
	return (StringLength(s) <= 0);
}

Bool stringEqualNotCaseSens(String wordA, String wordB){
	String wordALower = ConvertToLowerCase(wordA);
	String wordBLower = ConvertToLowerCase(wordB);

	Bool result = StringEqual(wordALower, wordBLower);
	FreeBlock(wordALower);
	FreeBlock(wordBLower);
	return result;
}

// Ckeck if every character is a digit
Bool stringIsNumber(String s){
	if (*s == '-') // 's' could be a negative value
		s++;
	while (*s){
		if (!isdigit((unsigned char)*s++))
			return FALSE;
	}
	return TRUE;
}

// Check if every character is a letter
Bool stringIsAlpha(String s){
	while (*s){
		if (!(*s >= 'A' && *s <= 'Z') && !(*s >= 'a' && *s <= 'z') && !(*s >= 'ä' && *s <= 'ö') && !(*s >= 'Ä' && *s <= 'Ö')) // Because couldn't get isAlpha to register åäö, ÅÄÖ
			return FALSE;
		s++;
	}
	return TRUE;
}