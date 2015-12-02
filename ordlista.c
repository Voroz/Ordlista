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
//#define DEBUG_ON

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
	pVector->data = GetBlock(sizeof(void*)* pVector->capacity);
}

int vectorSize(Vector *pVector){
	return pVector->size;
}

static void* vectorCopyValue(void* value, int sizeOfElem){
	void* ptr = GetBlock(sizeOfElem);
	memcpy(ptr, value, sizeOfElem);
	return ptr;
}

static void vectorFreeValue(void* value){
	free(value);
}

static void vectorDoubleCapacityIfFull(Vector *pVector){
	if (pVector->size >= pVector->capacity){

		void **newMemory = realloc(pVector->data, sizeof(void*)* (pVector->capacity * 2));
		if (newMemory){
#ifdef DEBUG_ON
			printf("Vector resize: %d to %d\n", pVector->size, (pVector->capacity * 2));
#endif
			pVector->data = newMemory;
			pVector->capacity *= 2;
		}
	}
}
// TODO: Call free word function for each word
static void vectorHalfCapacityIfNotUsed(Vector *pVector){
	if (pVector->capacity / pVector->size >= 2){

		void **newMemory = realloc(pVector->data, sizeof(void*)* (pVector->capacity / 2));
		if (newMemory){
#ifdef DEBUG_ON
			printf("Vector resize: %d to %d\n", pVector->capacity, pVector->size);
#endif
			pVector->data = newMemory;
			pVector->capacity /= 2;
		}
	}
}

// TODO: Refactor void* ptr and memcpy() line to a new static function.
void vectorAppend(Vector *pVector, void *value, int sizeOfElem){
	// Make sure there's room to expand into
	vectorDoubleCapacityIfFull(pVector);

	// Append the value and increment vector->size
	pVector->data[pVector->size++] = vectorCopyValue(value, sizeOfElem);
}

int vectorSet(Vector *pVector, int index, void *value){
	if (index >= pVector->size || index < 0){
#ifdef DEBUG_ON
		printf("'vectorSet' - Index %d is out of bounds for vector of size %d\n", index, pVector->size);
#endif
		return -1;
	}
	// Set the value at the desired index
	pVector->data[index] = value;
	return 1;
}

void* vectorGet(Vector *pVector, int index){
	if (index >= pVector->size || index < 0){
#ifdef DEBUG_ON
		printf("'vectorGet' - Index %d is out of bounds for vector of size %d\n", index, pVector->size);
#endif
	}
	return pVector->data[index];
}

// TODO: Refactor void* ptr and memcpy() line to a new static function.
//
// Moves all values one step higer from index position and inserts the new value at index
//
int vectorInsert(Vector *pVector, int index, void *value, int sizeOfElem){
	// Check if out of bounds
	if (index < 0 || index >= pVector->size){
#ifdef DEBUG_ON
		printf("'vectorInsert' - Index %d is out of bounds for vector of size %d\n", index, pVector->size);
#endif
		return -1;
	}
	// Make the vector one element larger to make room for the new value
	vectorAppend(pVector, pVector->data[pVector->size - 1], strlen(pVector->data[pVector->size - 1]) + 1);

	// Move all values whos index is larger than 'index' one higher (element 5 becomes 4, 4 becomes 3, and so on)
	for (int i = (pVector->size - 1); i > index; i--){
		vectorSet(pVector, i, pVector->data[i - 1]);
	}
	// Save the value at a new adress
	void* ptr = vectorCopyValue(value, sizeOfElem);
	// Save the new adress in the vector
	vectorSet(pVector, index, ptr);

	return 1;
}

int vectorRemove(Vector *pVector, int index){
	// Check if out of bounds
	if (index < 0 || index >= pVector->size){
#ifdef DEBUG_ON
		printf("'vectorRemove' - Index %d is out of bounds for vector of size %d\n", index, pVector->size);
#endif
		return -1;
	}
	// Move all values whos index is larger than 'index' one lower (element 4 becomes 5, 5 becomes 6, and so on)
	for (int i = index; i < (pVector->size - 1); i++){
		vectorSet(pVector, i, pVector->data[i + 1]);
	}
	// Decrement vector->size and set last value to NULL 
	vectorSet(pVector, (--pVector->size), NULL);

	// Check usage and halves vector is usage is <= 50%
	vectorHalfCapacityIfNotUsed(pVector);
	return 1;
}

void vectorFree(Vector *pVector){
	for (int i = 0; i < pVector->size; i++){
		vectorFreeValue(pVector->data[i]);
	}
	free(pVector->data);
}

void vectorClear(Vector *pVector){
	vectorFree(pVector);
	vectorInit(pVector);
}

//###########################################################//
// TODO: Replace å,ä,ö and Å,Ä,Ö with
// \x86 = å
// \x84 = ä
// \x94 = ö

// \x8F = Å
// \x8E = Ä
// \x99 = Ö
// function(String word);

void appendFileExtension(String value, String extension){
	// Get the last charecters of 'value', exactly the amount of charecters in 'extension'
	String fileEnding = SubString(value, (strlen(value) - strlen(extension)), strlen(value));

	// If the string 'value' dosen't already end with 'ext' add 'ext'
	if (strcmp(fileEnding, extension)){
		String newExt = Concat(".", extension);
		strcat(value, newExt);
		FreeBlock(newExt);
	}
}

FILE *openFile(String filename, String accessMode){
	FILE *file;

	// Add txt extension to 'filename' if not already there
	appendFileExtension(filename, "txt");

	// Open file and check for errors
	if (!(file = fopen(filename, accessMode))){
		printf("Error opening %s\n", filename); // TODO: Move error message to scope of call, (switch command)
		return;
	}
	return file;
}

void closeFile(FILE *file){
	fclose(file);
}

int storeWordsFromFile(FILE *file, Vector *pVector){
	String word;

	// Temporary memory
	word = GetBlock(MAX_WORD_LENGTH);
	// Read word into temporary memory
	for (int i = 0; fscanf(file, "%s\n", word) != EOF; i++){
		// Append word to vector
		vectorAppend(pVector, word, (strlen(word) + 1));
	}
	free(word);

	return 1;
}

int saveWordsToFile(FILE *file, Vector *pVector){
	// Save words to file
	if (vectorSize(pVector) != 0){
		for (int i = 0; i < vectorSize(pVector) - 1; i++){
			fprintf(file, "%s\n", vectorGet(pVector, i));
		}
		fprintf(file, "%s", vectorGet(pVector, vectorSize(pVector) - 1));
	}
	else{
#ifdef DEBUG_ON
		printf("'saveWordsToFile' - Vector doesn't contain anything!\n");
#endif
		return -1;
	}
	return 1;
}

// TODO: Use strcmp() to find postion of word and return position
int findPosForWord(String wordA, String wordB){
	return memcmp(wordA, wordB, strlen(wordA));
}

// Find word and return word position
int getWordPos(String word, Vector *pVector){
	if (strlen(word) <= 0)
	{
#ifdef DEBUG_ON
		printf("findPosForWord: Word to search for is empty string\n");
#endif
		return -2;
	}
	for (int i = 0; i < vectorSize(pVector); i++){
		// Check if strings match with memcmp
		if (memcmp(word, vectorGet(pVector, i), strlen(word)) == 0){
			// i = position
			return i;
		}
	}
	return -1;
}

//TODO: Change to return Int* dynamic array with positions
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
		printf("'deleteWord' - Index %d is out of bounds for vector of size %d\n", index, pVector->size);
#endif
		return -1;
	}

	vectorRemove(pVector, index);
	printf("Ordet togs bort\n");
	return 1;
}

// Antagligen onÃ¶dig, bÃ¤ttre och kÃ¶ra deleteWord() i en loop
void deleteManyWords(int index, int numWords, Vector *pVector){
	for (int i = index; i < (index + numWords); i++){
		vectorRemove(pVector, index);
	}
}

// TODO: Check if word is an int and disallow user to add word.
int addWord(String word, int index, Vector *pVector){
	// Check if out of bounds
	if (index < 0 || index > vectorSize(pVector)){
#ifdef DEBUG_ON
		printf("'addWord' - Index %d is out of bounds for vector of size %d\n", index, pVector->size);
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
		printf("'editWord' - Index %d is out of bounds for vector of size %d\n", index, pVector->size);
#endif
		return -1;
	}
	printf("The word you are about to edit: %s:\n", vectorGet(pVector, index));
	wordToEdit = GetLine();
	// TODO: Check if wordToEdit is empty string and dont save it and return -2
	vectorSet(pVector, index, wordToEdit);
	return 1;
}

void printToScreen(String word, int position){
	printf("\n%d\t%s", position, word);
}

int printWordsInVector(Vector *pVector, int startIndex, int numberOfWords){
	// Check if out of bounds
	if (startIndex < 0 || startIndex >= vectorSize(pVector)){
#ifdef DEBUG_ON
		printf("printWordsInVector: startIndex %d is out of bounds for vector of size %d\n", startIndex, pVector->size);
#endif
		return -1;
	}
	if ((startIndex + numberOfWords) > vectorSize(pVector))
	{
#ifdef DEBUG_ON
		printf("printWordsInVector: startIndex %d + numberOfWords %d will print data out of bounds for vector of size %d\n", startIndex, numberOfWords, pVector->size);
#endif
		return -2;
	}

	for (int i = startIndex; i < startIndex + numberOfWords; i++){
		printf("\n%d\t%s", i, vectorGet(pVector, i));
	}
	return 1;
}

int printHelpInfo(){
	printf("*******************************************************\n");
	printf("Here are the commands available in the program.\n\n\n");
	printf("To add a word, type:\t\tadd\tex: add Giraff\n");
	printf("To edit a word, type:\t\tedit\tex: edit Giraff\n");
	printf("To delete a word, type:\t\tdelete\tex: delete Giraff or delete 42\n");
	printf("To find a word, type:\t\tfind\tex: find Giraff or find a\n");
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
	load,
	save,
	exitProg
};

int readCommand(String value){
	if (value[0] == 'h' && StringEqual("help", ConvertToLowerCase(value))){
		return help;
	}
	if (value[0] == 'a' && StringEqual("add", ConvertToLowerCase(value))){
		return add;
	}
	if (value[0] == 'd' && StringEqual("delete", ConvertToLowerCase(value))){
		return delete;
	}
	if (value[0] == 'f' && StringEqual("find", ConvertToLowerCase(value))){
		return find;
	}
	if (value[0] == 'p' && StringEqual("print", ConvertToLowerCase(value))){
		return print;
	}
	if (value[0] == 'l' && StringEqual("load", ConvertToLowerCase(value))){
		return load;
	}
	if (value[0] == 's' && StringEqual("save", ConvertToLowerCase(value))){
		return save;
	}
	if (value[0] == 'e'){
		if (StringEqual("edit", ConvertToLowerCase(value))){
			return edit;
		}
		if (StringEqual("exit", ConvertToLowerCase(value))){
			return exitProg;
		}
	}

	return 0;
}

// TODO: switch all input from user to lower case.
void readInput(String *pCommand, String *pValue){
	printf("\n\n%c", '>');

	String userInput,
		commandInput,
		valueInput;

	userInput = GetLine();

	int spaceChar = (FindChar(' ', userInput, 0));
	if (spaceChar == -1){
		commandInput = SubString(userInput, 0, StringLength(userInput));
		free(*pCommand);
		*pCommand = commandInput;
		*pValue = '\0';
	}
	else {
		commandInput = SubString(userInput, 0, (spaceChar - 1));
		free(*pCommand);
		*pCommand = commandInput;

		valueInput = SubString(userInput, (spaceChar + 1), StringLength(userInput));
		free(*pValue);
		*pValue = valueInput;
	}
	free(userInput);
}

int switchCommand(String command, String value, Vector *pVector) {
	int number;
	Vector pCompareVector;
	switch (readCommand(command)){
	case (help) :
		printHelpInfo();
		return 1;

	case (add) :
		addWord(value, 2, pVector);
		return 1;

	case (delete) :
		// Transform value to int, returns -1 if it failed
		number = StringToInteger(value);
		// If number is a real number
		if (number > -1){
			deleteWord(number, pVector);
			return 1;
		}
		deleteWord(getWordPos(value, pVector), pVector);
		return 1;

	case (edit) :
		// Transform value to int, returns -1 if it failed
		number = StringToInteger(value);
		// If number is a real number
		if (number > -1){
			editWord(number, pVector);
			return 1;
		}
		editWord(getWordPos(value, pVector), pVector);
		return 1;

	case (find) :
		pCompareVector = searchForWords(value, pVector);
		if (vectorSize(&pCompareVector) > 0) {
			printWordsInVector(&pCompareVector, 0, vectorSize(&pCompareVector));
		}
		vectorFree(&pCompareVector);
		return 1;

	case (print) :
		printWordsInVector(pVector, 0, vectorSize(pVector));
		return 1;

	case (load) :
		vectorClear(pVector);
		FILE *fileToLoad = openFile(value, "r");
		storeWordsFromFile(fileToLoad, pVector);
		closeFile(fileToLoad);
		return 1;

	case (save) :
	{
		FILE *fileToSave = openFile(value, "w");
		int result = saveWordsToFile(fileToSave, pVector);
		closeFile(fileToSave);
		if (result == -1){
			printf("You have nothing to save idiot! ;)");
		};
		return 1;
	}
	case (exitProg) :
		return 0;

	default:
		printf("Error: Command doesn't exist. Try again.");
		return -1;
	}

	return -1;
}


//###########################################################//
int main()
{
	String command = GetBlock(MAX_WORD_LENGTH), value = GetBlock(MAX_WORD_LENGTH);
	Vector container;
	vectorInit(&container);

	int check = 1;
	while (check){
		readInput(&command, &value);
		check = switchCommand(command, value, &container);
	}

	return 0;
}
