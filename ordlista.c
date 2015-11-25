#define _CRT_SECURE_NO_WARNINGS


#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <String.h>
#include <ctype.h>
#include "genlib.h"
#include "random.h"
#include "simpio.h"
#include "strlib.h"

#define VECTOR_INITIAL_CAPACITY 10
#define MAX_WORD_LENGTH 100


typedef struct{
	String word;
	int length;
} Word;

typedef struct{
	int size;		// slots used so far
	int capacity;	// total available slots
	Word *data;		// array of integers we're storing
} Vector;



//###########################################################//
void vectorInit(Vector *pVector) {
	// initialize size and capacity
	pVector->size = 0;
	pVector->capacity = VECTOR_INITIAL_CAPACITY;

	// allocate memory for vector->data
	pVector->data = GetBlock(sizeof(Word)* pVector->capacity);
}

void vectorDoubleCapacityIfFull(Vector *pVector) {
	if (pVector->size >= pVector->capacity) {
		// double vector->capacity and resize the allocated memory accordingly
		pVector->capacity *= 2;
		pVector->data = realloc(pVector->data, sizeof(Word)* pVector->capacity);
	}
}

void vectorHalfCapacityIfNotUsed(Vector *pVector) {
	if (pVector->capacity / pVector->size >= 2) {
		// half vector->capacity and resize the allocated memory accordingly
		pVector->capacity /= 2;
		pVector->data = realloc(pVector->data, sizeof(Word)* pVector->capacity);
	}
}

void vectorAppend(Vector *pVector, String value) {
	// make sure there's room to expand into
	vectorDoubleCapacityIfFull(pVector);

	// append the value and increment vector->size
	pVector->data[pVector->size].length = (strlen(value) + 1);
	pVector->data[pVector->size].word = GetBlock(pVector->data[pVector->size].length);
	memcpy(pVector->data[pVector->size].word, value, pVector->data[pVector->size].length);
	pVector->size++;
}

void vectorSet(Vector *pVector, int index, String value) {
	// zero fill the vector up to the desired index
	while (index >= pVector->size) {
		vectorAppend(pVector, "");
	}

	free(pVector->data[index].word);

	// set the value at the desired index
	pVector->data[index].length = (strlen(value) + 1);
	pVector->data[index].word = GetBlock(pVector->data[index].length);
	memcpy(pVector->data[index].word, value, pVector->data[index].length);
}

Word vectorGet(Vector *pVector, int index) {
	if (index >= pVector->size || index < 0) {
		printf("Index %d is out of bounds for vector of size %d\n", index, pVector->size);
		exit(1);
	}
	return pVector->data[index];
}

void vectorInsert(Vector *pVector, int index, String value) {
	// make sure there's room to expand into
	vectorDoubleCapacityIfFull(pVector);

	for (int i = pVector->size; i > index; i--) {
		vectorSet(pVector, i, pVector->data[i - 1].word);
	}

	vectorSet(pVector, index, value);
}

void vectorRemove(Vector *pVector, int index) {

	for (int i = index; i < (pVector->size - 1); i++) {
		vectorSet(pVector, i, pVector->data[i + 1].word);
	}
	pVector->size--;

	free(pVector->data[pVector->size].word);
	vectorHalfCapacityIfNotUsed(pVector);
}



//###########################################################//
void storeWordsFromFile(String filename, Vector *pVector) {
	FILE *file;
	String word;

	// Open file
	if (!(file = fopen(filename, "r"))) {
		printf("Error opening %s\n", filename);
		return;
	}
	// Temporary memory
	word = malloc(MAX_WORD_LENGTH);	

	// Read word into temporary memory
	for (int i = 0; fscanf(file, "%s\n", word) != EOF; i++) {
		vectorAppend(pVector, word);
	}
	free(word);
	fclose(file);
}

int compareWords(String wordA, String wordB) {
	memcmp(wordA, wordB, strlen(wordA));
}

//Find word and return word position
int findPosForWord(String word, Vector *pVector) {
	for (int i = 0; i < pVector->size; i++){
		//Check if strings match with memcmp
		if (memcmp(word, pVector->data[i].word, pVector->data[i].length) == 0){
			return i; // i = position
		}
	}
	return -1;
}

int* searchForWords(String searchTerm, Vector *pVector) {

	int *pCompareVector = calloc(pVector->size, sizeof(int));
	
	for (int i = 0; i < pVector->size; i++) {
		if (strstr(pVector->data[i].word, searchTerm) != NULL) {
			pCompareVector[i]++;
		}
	}
	return pCompareVector;
}

int deleteWord(int index, Vector *pVector) {
	//Check if out of bounds
	if (index < 0 || index >= pVector->size){
		printf("Index %d is out of bounds for vector of size %d\n", index, pVector->size);
		return -1;
	}

	vectorRemove(pVector, index);
	printf("Ordet togs bort\n");
	return 1;
}

// Antagligen onödig, bättre och köra deleteWord() i en loop
void deleteManyWords(int index, int numWords, Vector *pVector) {
	for (int i = index; i < (index + numWords); i++)
	{
		vectorRemove(pVector, index);
	}
}

int addWord(String word, int index, Vector *pVector) {
	//Check if out of bounds
	if (index < 0 || index >= pVector->size) {
		printf("Index %d is out of bounds for vector of size %d\n", index, pVector->size);
		return -1;
	}

	vectorInsert(pVector, index, word);
	printf("Ordet la tills\n");
	return 1;
}

void printToScreen(Word word, int position)
{
	printf("\n%d\t%s", position, word.word);
}



//###########################################################//
typedef enum {
	add = 297,
	delete = 627,
	find = 417
};

int addAsciiChars(String value) {
	int number = 0;
	for (int i = 0; value[i] != 0; i++)
	{
		number += (int)value[i];
	}
	return number;
}


int readCommand(String value) {
	if (*value == 104)
	{
		if (StringEqual("help", ConvertToLowerCase(value))) {
			return 1;
		}
	}
	if (*value == 97)
	{
		if (StringEqual("add", ConvertToLowerCase(value))) {
			return 2;
		}
	}
	if (*value == 100)
	{
		if (StringEqual("delete", ConvertToLowerCase(value))) {
			return 3;
		}
	}
	if (*value == 102)
	{
		if (StringEqual("find", ConvertToLowerCase(value))) {
			return 4;
		}
	}
	if (*value == 112)
	{
		if (StringEqual("print", ConvertToLowerCase(value))) {
			return 5;
		}
	}
	if (*value == 101)
	{
		if (StringEqual("exit", ConvertToLowerCase(value))) {
			return 6;
		}
	}
	return 0;
}

void readInput(String command, String value) {
	printf("\n\n%c", '>');
	String userInput,
		commandInput,
		valueInput;

	userInput = GetLine();
	int spaceChar = (FindChar(' ', userInput, 0));
	if (spaceChar == -1)
	{
		commandInput = SubString(userInput, 0, StringLength(userInput));
		valueInput = "\0";
	}
	else
	{
		commandInput = SubString(userInput, 0, (spaceChar - 1));
		valueInput = SubString(userInput, (spaceChar + 1), StringLength(userInput));
		memcpy(value, valueInput, StringLength(userInput));
		FreeBlock(valueInput);
	}

	memcpy(command, commandInput, StringLength(userInput) + 1);

	FreeBlock(commandInput);
}


int switchCommand(String command, String value, Vector *pVector) {
	switch (readCommand(command)) {
	case (1) :
		//printHelpInfo();
		break;
	case (2) :
		addWord(value, 10, pVector);
		return 1;
		break;
	case (3) :
	{
		// Transform value to int, returns -1 if it failed
		int number = StringToInteger(value);
		// If number is a real number 
		if (number > -1) {
			deleteWord(number, pVector);
			return 1;
		}
		deleteWord(findPosForWord(value, pVector), pVector);
		return 1;
		break;
	}
	case (4) :
	{
		int *pCompareVector = searchForWords(value, pVector);
		for (int i = 0; i < pVector->size; i++) {
			if (pCompareVector[i] > 0) {
				printToScreen(pVector->data[i], i);
			}
		}
		free(pCompareVector);
		return 1;
		break;
	}
	case (5) :
	{
		for (int i = 0; i < pVector->size; i++) {
			printToScreen(pVector->data[i], i);
		}
		return 1;
		break;
				 
	}
	case (6) :
		return 0;
		break;

	default:
		printf("Error: Command doesn't exist. Try again.");
		return -1;
		break;
	}
}



//###########################################################//
int main()
{
	String command = GetBlock(MAX_WORD_LENGTH), value = GetBlock(MAX_WORD_LENGTH);
	Vector vector;
	vectorInit(&vector);
	storeWordsFromFile("Ordlista.txt", &vector);

	//vectorSet(&vector, 25, "-hej-");
	//vectorSet(&vector, 89, "Coca-Cola");

	//vectorRemove(&vector, 87);


	//vectorInsert(&vector, 5, "--test--");
	//vectorInsert(&vector, 10, "--test--");
	//vectorInsert(&vector, 16, "--test--");

	//deleteWord(86, &vector);
	//deleteManyWords(findPosForWord("Allrum", &vector), 10, &vector);
	//deleteWord(findPosForWord("Hjälpsam", &vector), &vector);

	int check = 1;
	while (check)
	{
		readInput(command, value);
		check = switchCommand(command, value, &vector);
	}

	
	//printf("findPosForWord returned: %d for word \"Brasa\".\n\n", findPosForWord("Brasa", &vector));

	//vectorRemove(&vector, 5);

	//printf("%s", addWord("--test addWord--", 45, &vector));


	//Word a = vectorGet(&vector, 29);
	//printf("\nvectorGet returned: %s", a.word);
}
