#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "genlib.h"
#include "strlib.h"

char convertToSweChar(int input){
	//�
	if (input == -122){
		return -27;
	}
	//�
	if (input == -124){
		return -28;
	}
	//�
	if (input == -108){
		return -10;
	}
	//�
	if (input == -113){
		return -59;
	}
	//�
	if (input == -114){
		return -60;
	}
	//�
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

Bool stringIsNumber(String s){
	if (*s == '-') // 's' could be a negative value
		s++;
	while (*s){
		if (!isdigit((unsigned char)*s++))
			return FALSE;
	}
	return TRUE;
}

Bool stringIsAlpha(String s){
	while (*s){
		if (!(*s >= 'A' && *s <= 'Z') && !(*s >= 'a' && *s <= 'z') && !(*s >= '�' && *s <= '�') && !(*s >= '�' && *s <= '�')) // Because couldn't get isAlpha to register ���, ���
			return FALSE;
		s++;
	}
	return TRUE;
}

void stringFormat(String s){
	int chr = 0;

	if (s == NULL) Error("NULL String passed to stringFormat");

	convertToSweString(s);

	while (s[chr]){
		s[chr] = tolower(s[chr]);
		chr++;
	}
	s[0] = toupper(s[0]);
}

void compareSwedish(String s){
	for (int chr = 0; chr < StringLength(s); chr++){ // To get program to sort '�' and '�' in correct order according to swedish alphabet
		if (s[chr] == '�' || s[chr] == '�'){
			s[chr] += 2;
		}
	}
}