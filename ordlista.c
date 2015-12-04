#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <String.h>
#include <ctype.h>
#include <locale.h>
#include <stdarg.h>
#include "genlib.h"
#include "random.h"
#include "simpio.h"
#include "strlib.h"

#define VECTOR_GET(vec, type, id) *((type) vectorGet(&vec, id))

#define VECTOR_INITIAL_CAPACITY 10
#define MAX_WORD_LENGTH 100
#define DEBUG_ON

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

static void** vectorCopyValue(void* *value, int sizeOfElem){
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

void vectorAppend(Vector *pVector, void* *value, int sizeOfElem){
	// Make sure there's room to expand into
	vectorDoubleCapacityIfFull(pVector);

	// Append the value and increment vector->size
	pVector->data[pVector->size++] = vectorCopyValue(value, sizeOfElem);
	return;
}

void vectorSet(Vector *pVector, int index, void* *value){
	if (index >= pVector->size || index < 0){
		#ifdef DEBUG_ON
			printf("'vectorSet' - Index %d is out of bounds for vector of size %d\n", index, pVector->size);
		#endif
	}
	// Set the value at the desired index
	pVector->data[index] = value;
}

void* vectorGet(Vector *pVector, int index){
	if (index >= pVector->size || index < 0){
		#ifdef DEBUG_ON
			printf("'vectorGet' - Index %d is out of bounds for vector of size %d\n", index, pVector->size);
		#endif
	}
	return pVector->data[index];
}

//
// Moves all values one step higer from index position and inserts the new value at index
//
void vectorInsert(Vector *pVector, int index, void* *value, int sizeOfElem){
	// Check if out of bounds
	if (index < 0 || index >= pVector->size){
		#ifdef DEBUG_ON
			printf("'vectorInsert' - Index %d is out of bounds for vector of size %d\n", index, pVector->size);
		#endif
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
}

void vectorRemove(Vector *pVector, int index){
	// Check if out of bounds
	if (index < 0 || index >= pVector->size){
		#ifdef DEBUG_ON
			printf("'vectorRemove' - Index %d is out of bounds for vector of size %d\n", index, pVector->size);
		#endif
	}
	// Remove value att index position
	vectorFreeValue(pVector->data[index]);
	// Move all values whos index is larger than 'index' one lower (element 4 becomes 5, 5 becomes 6, and so on)
	for (int i = index; i < (pVector->size - 1); i++){
		vectorSet(pVector, i, pVector->data[i + 1]);
	}
	// Decrement vector->size and set last value to NULL
	vectorSet(pVector, (pVector->size - 1), NULL);
	pVector->size--;

	// Check usage and halves vector if usage is <= 50%
	vectorHalfCapacityIfNotUsed(pVector);
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
typedef enum errors{
	general,
	fileOpen,
	saveEmpty,
	noWordFound,
	nullWord,
	alreadyExist,
	notAllowedChar
}ErrorCode;

void userError(ErrorCode err, String s, ...){

	va_list args;
	va_start(args, s);

	switch (err)
	{
	case (general) :
		printf("%s", "Something went wrong");
		break;
	case (fileOpen) :
		printf("Error opening %s\n", s);
		break;
	default:
		break;
	}

	vfprintf(stdout, "%s", args);

	va_end(args);

}

//###########################################################//
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
    if (input == -98){
        return -60;
    }
    //Ö
    if (input == -103){
        return -42;
    }
    return;
}

void convertToSweString(String input){
    for (int i = 0; i < strlen(input); i++){
        input[i] = convertToSweChar(input[i]);
    }
}

String findExtension(String filename){
	return SubString(filename, FindCharFromRight('.', filename, 0), strlen(filename));
}

void appendFileExtension(String filename, String extension){
	//Exit if there's already an extension
	if (findExtension(filename)[0] == '.'){
		#ifdef DEBUG_ON
			printf("'appendFileExtension' - File already has an extension.\n");
		#endif
		return;
	}
	strcat(filename, extension);
}

FILE *openFile(String filename, String accessMode){
	FILE *file;
	// Add txt extension to 'filename' if not already there
	appendFileExtension(filename, ".txt");

	// Open file and check for errors
	if (!(file = fopen(filename, accessMode))){
		userError(fileOpen, filename, "hej", "detta gick inte");
		return NULL;
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
			printf("'saveWordsToFile' - Vector doesn't contain anything\n");
		#endif
		return -1;
	}
	return 1;
}

int findPosForWord(String word, Vector *pVector){
	String wordInVector = "";
	int i;
	for (i = 0; i < pVector->size; i++){       // (->size används för att for-loopen ska veta att det är antalet ord i pVectorn som ska loopas igenom. dvs 87 stycken.)
		wordInVector = ConvertToLowerCase(vectorGet(pVector, i)); // (->data[i] används för att det är ordet på det indexet som vi ska jämföra med.)
		if (StringCompare(word, wordInVector) < 0){
			FreeBlock(wordInVector);
			return i;
		}
		FreeBlock(wordInVector);
	}
	return i;
}


// Find word and return word position
int getWordPos(String word, Vector *pVector){
	if (strlen(word) <= 0){
		#ifdef DEBUG_ON
			printf("findPosForWord: Word to search for is empty string\n");
		#endif
		return -2;
	}
	for (int i = 0; i < vectorSize(pVector); i++){
		String wordInVector = ConvertToLowerCase(vectorGet(pVector, i));
		// Check if strings match
		if (StringEqual(word, wordInVector)){
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
		String wordInVector = ConvertToLowerCase(vectorGet(pVector, i));
		if (strstr(wordInVector, searchTerm) != NULL){
			vectorAppend(&pCompareVector, &i, sizeof(int));
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
	printf("The word has been deleted.\n");
	return 1;
}

// Antagligen onödig, bättre och köra deleteWord() i en loop
void deleteManyWords(int index, int numWords, Vector *pVector){
	for (int i = index; i < (index + numWords); i++){
		vectorRemove(pVector, index);
	}
}

bool StringEqualNotCaseSens(String wordA, String wordB){
	String wordALower = ConvertToLowerCase(wordA);
	String wordBLower = ConvertToLowerCase(wordB);

	bool result = StringEqual(wordALower, wordBLower);
	FreeBlock(wordALower);
	FreeBlock(wordBLower);
	return result;
}

// TODO: Accept swedish chars as new word, dosen't work with åäö ÅÄÖ
// TODO: Make first char upper case
int addWord(String word, int index, Vector *pVector){
	// Check if out of bounds
	if (index < 0 || index > vectorSize(pVector)){
		#ifdef DEBUG_ON
			printf("'addWord' - Index %d is out of bounds for vector of size %d\n", index, pVector->size);
		#endif
		return -1;
	}
	if (StringToInteger(word) != -1){
		//Error: can not add number as a word
		return -2;
	}
	// Check if word is empty string
	if (strlen(word) <= 0){
		return -3;
	}
	// Check if word exist
	if (index > 0 && StringEqualNotCaseSens(word, vectorGet(pVector, index - 1))){
		return -4;
	}

	// If we're adding word to back of vector
	if (index == vectorSize(pVector)){
		vectorAppend(pVector, word, (strlen(word) + 1));
		printf("The word has been added.\n");
		return 1;
	}
	// If we're inserting word
	vectorInsert(pVector, index, word, (strlen(word) + 1));
	printf("The word has been added.\n");
	return 1;
}

// TODO: Accept swedish chars as new word, dosen't work with åäö ÅÄÖ
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
	// Check if word exist
	for (int i = 0; i < pVector->size; i++){
		if (StringEqualNotCaseSens(wordToEdit, vectorGet(pVector, i))){
			return -3;
		}
	}
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
	if ((startIndex + numberOfWords) > vectorSize(pVector)){
		#ifdef DEBUG_ON
			printf("printWordsInVector: startIndex %d + numberOfWords %d will print data out of bounds for vector of size %d\n", startIndex, numberOfWords, pVector->size);
		#endif
		return -2;
	}

	for (int i = startIndex; i < startIndex + numberOfWords; i++){
		printToScreen(vectorGet(pVector, i), i);
	}
	return 1;
}

int printHelpInfo(){
	printf("*******************************************************\n");
	printf("Here are the commands available in the program.\n\n\n");
	printf("To load a file, type:\t\tload\tex: load file\n");
	printf("To save the file, type:\t\tsave\tex: save file\n");
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
enum commands{
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

int readCommand(String command){
	if (command[0] == 'h' && StringEqual("help", command)){
		return help;
	}
	if (command[0] == 'a' && StringEqual("add", command)){
		return add;
	}
	if (command[0] == 'd' && StringEqual("delete", command)){
		return delete;
	}
	if (command[0] == 'f' && StringEqual("find", command)){
		return find;
	}
	if (command[0] == 'p' && StringEqual("print", command)){
		return print;
	}
	if (command[0] == 'l' && StringEqual("load", command)){
		return load;
	}
	if (command[0] == 's' && StringEqual("save", command)){
		return save;
	}
	if (command[0] == 'e'){
		if (StringEqual("edit", command)){
			return edit;
		}
		if (StringEqual("exit", command)){
			return exitProg;
		}
	}

	return 0;
}

//TODO: Comment this code.
void readInput(String command, String value){
	String userInputRaw,
		userInput,
		commandInput,
		valueInput;

	userInputRaw = GetLine();
	userInput = ConvertToLowerCase(userInputRaw);
	free(userInputRaw);

	int spaceChar = (FindChar(' ', userInput, 0));
	if (spaceChar == -1){
		commandInput = SubString(userInput, 0, StringLength(userInput));
		value = '\0';
	}
	else {
		commandInput = SubString(userInput, 0, (spaceChar - 1));
		valueInput = SubString(userInput, (spaceChar + 1), StringLength(userInput));

		memcpy(value, valueInput, (StringLength(valueInput) + 1));
		FreeBlock(valueInput);
	}
	memcpy(command, commandInput, (StringLength(commandInput) + 1));
	FreeBlock(commandInput);
	FreeBlock(userInput);
}

int switchCommand(String command, String value, Vector *pVector) {
    convertToSweString(value);
	Vector pCompareVector;
	FILE *fileToLoad;
	FILE *fileToSave;
	int number;
	switch (readCommand(command)){

	case (help) :
		printHelpInfo();
		return 1;

	case (add) :
		addWord(value, findPosForWord(value, pVector), pVector);
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

		//TODO: the Error messages doesnt work properly.
	case (edit) :
		// Transform value to int, returns -1 if it failed
		number = StringToInteger(value);
		int result;
		// If number is a real number
		if (number > -1){
			result = editWord(number, pVector);
		}
		else{
			result = editWord(getWordPos(value, pVector), pVector);
		}

		switch (result)
		{
		case 1:
			printf("The word was succsesfully changed.");
			break;
		case -1:
			printf("Index is out of bounds for file.");
			break;
		case -2:
			printf("The word doesn't exist.");
			break;
		case -3:
			printf("The word does already exist");
			break;
		default:
			break;
		}
		return 1;
	case (find) :
		pCompareVector = searchForWords(value, pVector);
		if (vectorSize(&pCompareVector) > 0) {
			for (int i = 0; i < vectorSize(&pCompareVector); i++){
				int wordIndex = VECTOR_GET(pCompareVector, int*, i);
				String word = vectorGet(pVector, wordIndex);
				printToScreen(word, wordIndex);
			}
		}
		if (vectorSize(&pCompareVector) == -1) {
			printf("The word doesnt exist.");
			return 1;
		}
		vectorFree(&pCompareVector);
		return 1;

	case (print) :
		printWordsInVector(pVector, 0, vectorSize(pVector));
		return 1;

	case (load) :
		vectorClear(pVector);
		if (!(fileToLoad = openFile(value, "r"))){
			return -1;
		}

		storeWordsFromFile(fileToLoad, pVector);
		closeFile(fileToLoad);
		return 1;

	case (save) :
		if (!(fileToSave = openFile(value, "w"))){
			return -1;
		}
				switch (saveWordsToFile(fileToSave, pVector))
				{
				case 1:
					break;
				case -1:
					printf("You have nothing to save.");
					break;
				case -2:
					break;
				default:
					break;
				}
				closeFile(fileToSave);
				return 1;
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
	if (!setlocale(LC_ALL, "")) {
		printf("error while setting locale\n");
	}
	String command = GetBlock(MAX_WORD_LENGTH), value = GetBlock(MAX_WORD_LENGTH);
	Vector container;
	vectorInit(&container);

	FILE *ordlista = openFile("ordlista.txt", "r");
	storeWordsFromFile(ordlista, &container);

	printf("WordMagic ver 0.1\n");
	printf("To get started, type help.");
	int check = 1;
	while (check){
		printf("\n\n%c", '>');
		readInput(command, value);
		check = switchCommand(command, value, &container);
	}

	return 0;
}
