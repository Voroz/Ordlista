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
// #define DEBUG_ON

typedef struct{
	String string;
	int index;
} Word;

typedef struct{
	int size;		// Slots used so far
	int capacity;	// Total available slots
	void **data;	// Array of data we're storing
} Vector;


//###########################################################//
void vectorInit(Vector *pVector){
	// Initialize size and capacity
	pVector->size = 0;
	pVector->capacity = VECTOR_INITIAL_CAPACITY;

	// Allocate memory for vector->data
	pVector->data = GetBlock(sizeof(void*) * pVector->capacity);
}

int vectorSize(Vector *pVector){
	return pVector->size;
}

static void vectorDoubleCapacityIfFull(Vector *pVector){
#ifdef DEBUG_ON
	printf("Vector resize: %d to %d\n", pVector->size, pVector->capacity);
#endif
	if (pVector->size >= pVector->capacity){

		void **newMemory = realloc(pVector->data, sizeof(void*) * (pVector->capacity * 2));
		if (newMemory){
			pVector->data = newMemory;
			pVector->capacity *= 2;
		}
	}
}

static void vectorHalfCapacityIfNotUsed(Vector *pVector){
#ifdef DEBUG_ON
	printf("Vector resize: %d to %d\n", pVector->capacity, pVector->size);
#endif
	if (pVector->capacity / pVector->size >= 2){

		void **newMemory = realloc(pVector->data, sizeof(void*) * (pVector->capacity / 2));
		if (newMemory){
			pVector->data = newMemory;
			pVector->capacity /= 2;
		}
	}
}

void vectorAppend(Vector *pVector, void **value, int sizeOfElem){
	// Make sure there's room to expand into
	vectorDoubleCapacityIfFull(pVector);

	// Append the value and increment vector->size
	void* ptr = GetBlock(sizeOfElem);
	memcpy(ptr, value, sizeOfElem);
	pVector->data[pVector->size++] = ptr;
}

void vectorSet(Vector *pVector, int index, void **value){
	if (index >= pVector->size || index < 0){
        #ifdef DEBUG_ON
            printf("'vectorSet' - Index %d is out of bounds for vector of size %d\n", index, pVector->size);
		#endif
		return -1;
	}
	// Set the value at the desired index
	pVector->data[index] = value;
}

void *vectorGet(Vector *pVector, int index){
	if (index >= pVector->size || index < 0){
        #ifdef DEBUG_ON
            printf("'vectorGet' - Index %d is out of bounds for vector of size %d\n", index, pVector->size);
        #endif
		return -1;
	}
	return pVector->data[index];
}

//
// Moves all values one step higer from index position and inserts the new value at index
//
void vectorInsert(Vector *pVector, int index, void **value, int sizeOfElem){
    // Check if out of bounds
	if (index < 0 || index >= vectorSize(pVector)){
        #ifdef DEBUG_ON
            printf("Index %d is out of bounds for vector of size %d\n", index, pVector->size);
        #endif
		return -1;
	}

	// Make the vector one element larger to make room for the new value
	vectorAppend(pVector, pVector->data[pVector->size - 1], strlen(pVector->data[pVector->size - 1]) + 1);

	// Move all values whos index is larger than 'index' one step higher (3 becomes 2, 2 becomes 1, and so on)
	for (int i = (pVector->size - 1); i > index; i--){
		vectorSet(pVector, i, pVector->data[i - 1]);
	}
	// Save the value at a new adress
	void* ptr = GetBlock(sizeOfElem);
	memcpy(ptr, value, sizeOfElem);
	// Save the new adress in the vector
	vectorSet(pVector, index, ptr);
}

void vectorRemove(Vector *pVector, int index){
    // Check if out of bounds
	if (index < 0 || index >= vectorSize(pVector)){
        #ifdef DEBUG_ON
            printf("Index %d is out of bounds for vector of size %d\n", index, pVector->size);
        #endif
		return -1;
	}

	for (int i = index; i < (pVector->size - 1); i++){
		vectorSet(pVector, i, pVector->data[i + 1]);
	}
	vectorSet(pVector, (pVector->size - 1), NULL);
	pVector->size--;

	vectorHalfCapacityIfNotUsed(pVector);
}

void vectorFree(Vector *pVector){
	for (int i = 0; i < pVector->size; i++){
		free(pVector->data[i]);
	}
}

//###########################################################//
void storeWordsFromFile(String filename, Vector *pVector){
	FILE *file;
	String word;

	// Open file and check for errors
	if (!(file = fopen("Ordlista.txt", "r"))){
		printf("Error opening %s\n", "Ordlista.txt");
		return;
	}
	// Temporary memory
	word = malloc(MAX_WORD_LENGTH);
	// Read word into temporary memory
	for (int i = 0; fscanf(file, "%s\n", word) != EOF; i++){
		vectorAppend(pVector, word, (strlen(word) + 1));
	}
	free(word);
	fclose(file);
}

// Under heavy progress
int compareWords(String wordA, String wordB){
	return memcmp(wordA, wordB, strlen(wordA));
}

// Find word and return word position
int findPosForWord(String word, Vector *pVector){
	for (int i = 0; i < vectorSize(pVector); i++){
		// Check if strings match with memcmp
		if (memcmp(word, vectorGet(pVector, i), strlen(word)) == 0){
			// i = position
			return i;
		}
	}
	return -1;
}

Vector searchForWords(String searchTerm, Vector *pVector){

	Vector pCompareVector;
	vectorInit(&pCompareVector);

	for (int i = 0; i < vectorSize(pVector); i++){
		if (strstr(vectorGet(pVector, i), searchTerm) != NULL){
			vectorAppend(&pCompareVector, vectorGet(pVector, i), (strlen(vectorGet(pVector, i)) + 1));
		}
	}
	return pCompareVector;
}

int deleteWord(int index, Vector *pVector){
	// Check if out of bounds
	if (index < 0 || index >= vectorSize(pVector)){
        #ifdef DEBUG_ON
            printf("Index %d is out of bounds for vector of size %d\n", index, pVector->size);
        #endif
		return -1;
	}

	vectorRemove(pVector, index);
	printf("Ordet togs bort\n");
	return 1;
}

// Antagligen onödig, bättre och köra deleteWord() i en loop
void deleteManyWords(int index, int numWords, Vector *pVector){
	for (int i = index; i < (index + numWords); i++){
		vectorRemove(pVector, index);
	}
}

int addWord(String word, int index, Vector *pVector){
	// Check if out of bounds
	if (index < 0 || index > vectorSize(pVector)){
        #ifdef DEBUG_ON
            printf("Index %d is out of bounds for vector of size %d\n", index, pVector->size);
        #endif
		return -1;
	}

	// If we're adding word to back of vector
	if (index == vectorSize(pVector)){
		vectorAppend(pVector, word, (strlen(word) + 1));
		printf("Ordet las till\n");
		return 1;
	}

	// If we're inserting word
	vectorInsert(pVector, index, word, (strlen(word) + 1));
	printf("Ordet las till\n");
	return 1;
}

int editWord(int index, Vector *pVector){
	String wordToEdit = vectorGet(pVector, index);
	// Check if out of bounds
	if (index < 0 || index >= vectorSize(pVector)){
        #ifdef DEBUG_ON
            printf("editWord: Index %d is out of bounds for vector of size %d\n", index, pVector->size);
        #endif
		return -1;
	}
	printf("The word you are about to edit: %s:\n", vectorGet(pVector, index));
	wordToEdit = GetLine();
	// TODO: Check if wordToEdit is empty string and dont save it and return -1
	vectorSet(pVector, index, wordToEdit);
	return 1;
}

void printToScreen(String word, int position){
	printf("\n%d\t%s", position, word);
}

void printWordsInVector(Vector *pVector, int startIndex, int endIndex){
    // Check if out of bounds
	if (startIndex < 0 || startIndex >= vectorSize(pVector) ||
        endIndex >= vectorSize(pVector) || startIndex > endIndex){
        #ifdef DEBUG_ON
            printf("editWord: Index %d is out of bounds for vector of size %d\n", index, pVector->size);
        #endif
		return -1;
	}

    for (int i = startIndex; i <= endIndex; i++){
        printf("\n%d\t%s", i, vectorGet(pVector, i));
    }
}

int printHelpInfo(){
	printf("*******************************************************\n");
	printf("Here are the commands to navigate though our program.\n\n\n");
	printf("To add a word, type:\t\tadd\tex: \"add Giraff\"\n");
	printf("To edit a word, type:\t\tedit\tex: \"edit Giraff\"\n");
	printf("To delete a word, type:\t\tdelete\tex: \"delete Giraff\"\n");
	printf("To find a word, type:\t\tfind\tex: \"find Giraff\"\n");
	printf("To print the full list, type:\tprint\n");
	printf("To exit the program, type:\texit\n\n");
	printf("*******************************************************\n");
	return 1;
}

//###########################################################//
typedef enum {
	help = 1,
	add,
	edit,
	delete,
	find,
	print,
	exitProg
};

int readCommand(String value){
	if (*value == 'h' && StringEqual("help", ConvertToLowerCase(value))){
		return help;
	}
	if (*value == 'a' && StringEqual("add", ConvertToLowerCase(value))){
		return add;
	}
	if (*value == 'd' && StringEqual("delete", ConvertToLowerCase(value))){
		return delete;
	}
	if (*value == 'f' && StringEqual("find", ConvertToLowerCase(value))){
		return find;
	}
	if (*value == 'p' && StringEqual("print", ConvertToLowerCase(value))){
		return print;
	}
	if (*value == 'e'){
		if (StringEqual("edit", ConvertToLowerCase(value))){
			return edit;
		}
		if (StringEqual("exit", ConvertToLowerCase(value))){
			return exitProg;
		}
	}
	return 0;
}

void readInput(String command, String value){
	printf("\n\n%c", '>');
	String userInput,
		commandInput,
		valueInput;

	userInput = GetLine();

	int spaceChar = (FindChar(' ', userInput, 0));
	if (spaceChar == -1){
		commandInput = SubString(userInput, 0, StringLength(userInput));
		valueInput = "\0";
	}
	else {
		commandInput = SubString(userInput, 0, (spaceChar - 1));
		valueInput = SubString(userInput, (spaceChar + 1), StringLength(userInput));
		memcpy(value, valueInput, StringLength(userInput));
		FreeBlock(valueInput);
	}
	memcpy(command, commandInput, StringLength(userInput) + 1);
	FreeBlock(commandInput);
}

int switchCommand(String command, String value, Vector *pVector) {
	switch (readCommand(command)){
	case (help) :
		printHelpInfo();
		break;
	case (add) :
		addWord(value, 2, pVector);
		return 1;
		break;
	case (delete) :
	{
		// Transform value to int, returns -1 if it failed
		int number = StringToInteger(value);
		// If number is a real number
		if (number > -1){
			deleteWord(number, pVector);
			return 1;
		}
		deleteWord(findPosForWord(value, pVector), pVector);
		return 1;
		break;
	}
	case (edit) :
	{
		// Transform value to int, returns -1 if it failed
		int number = StringToInteger(value);
		// If number is a real number
		if (number > -1){
			editWord(number, pVector);
			return 1;
		}
		editWord(findPosForWord(value, pVector), pVector);
		return 1;
		break;
	}
	case (find) :
	{
		Vector pCompareVector = searchForWords(value, pVector);
		if (vectorSize(&pCompareVector) > 0) {
			printWordsInVector(&pCompareVector, 0, (vectorSize(&pCompareVector) - 1));
		}
		vectorFree(&pCompareVector);
		return 1;
		break;
	}
	case (print) :
	{
		printWordsInVector(pVector, 0, (vectorSize(pVector) - 1));
		return 1;
		break;

	}
	case (exitProg) :
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
	Vector container;
	vectorInit(&container);
	storeWordsFromFile("Ordlista.txt", &container);

	int check = 1;
	while (check){
		readInput(command, value);
		check = switchCommand(command, value, &container);
	}

	return 0;
}
