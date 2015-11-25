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


//-------------------------------------------------
void vectorInit(Vector *vector) {
	// initialize size and capacity
	vector->size = 0;
	vector->capacity = VECTOR_INITIAL_CAPACITY;

	// allocate memory for vector->data
	vector->data = malloc(sizeof(Word)* vector->capacity);
}

void vectorDoubleCapacityIfFull(Vector *vector) {
	if (vector->size >= vector->capacity) {
		// double vector->capacity and resize the allocated memory accordingly
		vector->capacity *= 2;
		vector->data = realloc(vector->data, sizeof(Word)* vector->capacity);
	}
}

void vectorHalfCapacityIfNotUsed(Vector *vector) {
	if (vector->capacity / vector->size >= 2) {
		// half vector->capacity and resize the allocated memory accordingly
		vector->capacity /= 2;
		vector->data = realloc(vector->data, sizeof(Word)* vector->capacity);
	}
}

void vectorAppend(Vector *vector, String value) {
	// make sure there's room to expand into
	vectorDoubleCapacityIfFull(vector);

	// append the value and increment vector->size
	vector->data[vector->size].length = (strlen(value) + 1);
	vector->data[vector->size].word = malloc(vector->data[vector->size].length);
	memcpy(vector->data[vector->size].word, value, vector->data[vector->size].length);
	vector->size++;
}

void vectorSet(Vector *vector, int index, String value) {
	// zero fill the vector up to the desired index
	while (index >= vector->size) {
		vectorAppend(vector, "");
	}

	free(vector->data[index].word);

	// set the value at the desired index
	vector->data[index].length = (strlen(value) + 1);
	vector->data[index].word = malloc(vector->data[index].length);
	memcpy(vector->data[index].word, value, vector->data[index].length);
}

Word vectorGet(Vector *vector, int index) {
	if (index >= vector->size || index < 0) {
		printf("Index %d is out of bounds for vector of size %d\n", index, vector->size);
		exit(1);
	}
	return vector->data[index];
}

void vectorInsert(Vector *vector, int index, String value) {
	// make sure there's room to expand into
	vectorDoubleCapacityIfFull(vector);

	for (int i = vector->size; i > index; i--) {
		vectorSet(vector, i, vector->data[i - 1].word);
	}

	vectorSet(vector, index, value);
}

void vectorRemove(Vector *vector, int index) {

	for (int i = index; i < (vector->size - 1); i++) {
		vectorSet(vector, i, vector->data[i + 1].word);
	}
	vector->size--;

	free(vector->data[vector->size].word);
	vectorHalfCapacityIfNotUsed(vector);
}


//-------------------------------------------------
void storeWordsFromFile(String filename, Vector *pVector) {
	FILE *file;
	String word;

	// Open file
	if (!(file = fopen(filename, "r")))
	{
		printf("Error opening %s\n", filename);
		return;
	}

	// Temporary memory
	word = malloc(MAX_WORD_LENGTH);	

	// Read word into temporary memory
	for (int i = 0; fscanf(file, "%s\n", word) != EOF; i++)
	{
		vectorAppend(pVector, word);
	}

	free(word);
	fclose(file);
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

	int *pCompareVector = (int)calloc(pVector->size, sizeof(int));
	
	for (int i = 0; i < pVector->size; i++)
	{
		if (strstr(pVector->data[i].word, searchTerm) != NULL)
		{
			pCompareVector[i]++;
		}
	}
	return pCompareVector;
}

String deleteWord(int index, Vector *vector) {
	//Check if out of bounds
	if (index < 0 || index >= vector->size){
		//printf("Index %d is out of bounds for vector of size %d\n", index, vector->size);
		return "Index is out of bounds for vector";
	}

	vectorRemove(vector, index);
	return "Ordet togs bort\n";
}

void deleteManyWords(int index, int numWords, Vector *pVector) {
	for (int i = index; i < (index + numWords); i++)
	{
		vectorRemove(pVector, index);
	}
}

String addWord(String word, int index, Vector *vector) {
	/*int distance = 1,
		amountOfWords = (*words)[0].size - position,
		originalSize = (*words)[0].size;*/

	//Check if out of bounds
	if (index < 0 || index >= vector->size) {
		return "Index %d is out of bounds for vector of size %d\n";
	}

	vectorInsert(vector, index, word);

	return "Ordet la tills\n";
}

void printToScreen(Word word, int position)
{
	printf("\n%d\t%s", position, word.word);
}


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
	if (*value == 97)
	{
		if (StringEqual("add", ConvertToLowerCase(value))) {
			return 1;
		}
	}
	if (*value == 100)
	{
		if (StringEqual("delete", ConvertToLowerCase(value))) {
			return 2;
		}
	}
	if (*value == 102)
	{
		if (StringEqual("find", ConvertToLowerCase(value))) {
			return 3;
		}
	}
	if (*value == 112)
	{
		if (StringEqual("print", ConvertToLowerCase(value))) {
			return 4;
		}
	}
	if (*value == 101)
	{
		if (StringEqual("exit", ConvertToLowerCase(value))) {
			return 5;
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
		valueInput = -1;
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
		printf("\n%s\n", addWord(value, 10, pVector));
		return 1;
		break;
	case (2) :
	{
		// Transform value to int, returns -1 if it failed
		int number = StringToInteger(value);
		// If number is a real number 
		if (number > -1) {
			printf("\n%s\n", deleteWord(number, pVector));
			return 1;
		}
		printf("\n%s\n", deleteWord(findPosForWord(value, pVector), pVector));
		return 1;
		break;
	}
	case (3) :
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
	case (4) :
	{
		for (int i = 0; i < pVector->size; i++) {
			printToScreen(pVector->data[i], i);
		}
		return 1;
		break;
				 
	}
	case (5) :
		return 0;
		break;

	default:
		printf("Error: Commando does not exist. Try again!");
		return -1;
		break;
	}
}


//-------------------------------------------------
int main()
{
	String command = malloc(MAX_WORD_LENGTH), value = malloc(MAX_WORD_LENGTH);
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
