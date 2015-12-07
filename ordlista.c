#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <stdio.h>
#include <String.h>
#include <ctype.h>
#include <locale.h>
#include <stdarg.h>
#include <setjmp.h>
#include "genlib.h"
#include "random.h"
#include "simpio.h"
#include "strlib.h"

#define VECTOR_INITIAL_CAPACITY 10
#define MAX_WORD_LENGTH 100
//#define DEBUG_ON

jmp_buf env;

int changes = 0;	// Keeps track of user changes to the vector

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
				printf("Vector resize: %d to %d\n", pVector->capacity, pVector->capacity / 2);
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
	pVector->size++;
	vectorDoubleCapacityIfFull(pVector);

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
	// Check usage and halves vector if usage is <= 50%
	vectorHalfCapacityIfNotUsed(pVector);

	// Remove value att index position
	vectorFreeValue(pVector->data[index]);
	// Move all values whos index is larger than 'index' one lower (element 4 becomes 5, 5 becomes 6, and so on)
	for (int i = index; i < (pVector->size - 1); i++){
		vectorSet(pVector, i, pVector->data[i + 1]);
	}
	// Decrement vector->size and set last value to NULL
	vectorSet(pVector, (pVector->size - 1), NULL);
	pVector->size--;
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
	noSearchResult,
	nullWord,
	alreadyExist,
	notAllowedChar,
	outOfBounds,
	noFilename,
	toLongInput,
	emptyList,
	unsavedChanges,
	unknownCommand
}ErrorCode;

void userError(ErrorCode err, ...){
	va_list args;
	va_start(args, err);

	switch (err){
	case (general) :
		fprintf(stderr, "\nSomething went wrong");
		break;
	case (fileOpen) :
		vfprintf(stderr, "\nError opening file %s, does it exist?", args);
		longjmp(env, 1);
	case (saveEmpty) :
		fprintf(stderr, "\nCan't save empty list to file");
		longjmp(env, 1);
	case (noWordFound) :
		vfprintf(stderr, "\nCan't find the word \"%s\" that you're looking for", args);
		longjmp(env, 1);
	case (noSearchResult) :
		vfprintf(stderr, "\nNo match searching for \"%s\"", args);
		longjmp(env, 1);
	case (nullWord) :
		fprintf(stderr, "\nCan't function with an empty word\n");
		longjmp(env, 1);
	case (alreadyExist) :
		vfprintf(stderr, "\nThe word \"%s\" already exist in the list", args);
		longjmp(env, 1);
	case (notAllowedChar) :
		vfprintf(stderr, "\nThe word \"%s\" is not valid", args);
		longjmp(env, 1);
	case (outOfBounds) :
		vfprintf(stderr, "\nThe index %d is out of bounds for your current list of size %d", args);
		longjmp(env, 1);
	case (noFilename) :
		fprintf(stderr, "\nNo filename specified");
		longjmp(env, 1);
	case (toLongInput) :
		fprintf(stderr, "\nInput is to long");
		longjmp(env, 1);
	case (emptyList) :
		vfprintf(stderr, "\nThe list is empty", args);
		break;
	case (unsavedChanges) :
		vfprintf(stderr, "\nYou have changes that are not saved \n- Type exit to discard changes \n- Press enter to return to program", args);
		break;
	case (unknownCommand) :
		vfprintf(stderr, "\nThe command \"%s\" is unknowm", args);
		break;
	default:
		break;
	}
	va_end(args);
}

typedef enum succes{
	wordAdded,
	wordDelete,
	wordEdit,
	loadFile,
	saveFile,
	clearList,
	continueProg
}SuccesCode;

void userSucces(SuccesCode succes, ...){
	va_list args;
	va_start(args, succes);

	switch (succes)
	{
	case (wordAdded) :
		vfprintf(stdout, "\nThe word \"%s\" has been added", args);
		break;
	case (wordDelete) :
		vfprintf(stdout, "\nThe word \"%s\" has been deleted", args);
		break;
	case (wordEdit) :
		vfprintf(stdout, "\nThe word \"%s\" has been changed to \"%s\"", args);
		break;
	case (loadFile) :
		vfprintf(stdout, "\nLoaded the file \"%s\"", args);
		break;
	case (saveFile) :
		vfprintf(stdout, "\nSaved the list to file \"%s\"", args);
		break;
	case (clearList) :
		vfprintf(stdout, "\nCleared the list", args);
		break;
	case (continueProg) :
		vfprintf(stdout, "\nGoing back to program", args);
		break;
	default:
		break;
	}
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
    return input;
}

void convertToSweString(String input){
	for (int i = 0; i < StringLength(input); i++){
        input[i] = convertToSweChar(input[i]);
    }
}

bool stringIsEmpty(String s){
	return (StringLength(s) <= 0);
}

bool stringEqualNotCaseSens(String wordA, String wordB){
	String wordALower = ConvertToLowerCase(wordA);
	String wordBLower = ConvertToLowerCase(wordB);

	bool result = StringEqual(wordALower, wordBLower);
	FreeBlock(wordALower);
	FreeBlock(wordBLower);
	return result;
}

// Ckeck if every character is a digit
bool stringIsNumber(String s){
	if (*s == '-'){
		s++;
	}
	while (*s){
		if (!isdigit((unsigned char)*s++)){
			return FALSE;
		} }
	return TRUE;
}

// Check if every character is a letter
bool stringIsAlpha(String s){
	while (*s){
		if (*s < -1){
			return FALSE;
		}
		if (!isalpha(*s++)){
			return FALSE;
		} }
	return TRUE;
}

String findExtension(String filename){
	return SubString(filename, FindCharFromRight('.', filename, 0), StringLength(filename));
}

// TODO: Try to make the memory situation better
// 'strcat' destination needs to be large enough to hold the new string
// Problem starts in 'openFile' with 'appendFileExtension'
void appendFileExtension(String *filename, String extension){
	int stringSize = StringLength(*filename) + StringLength(extension) + 2;
	String newFilename = GetBlock(stringSize);
	strcpy(newFilename, *filename);
	*filename = newFilename;

	String ext = findExtension(*filename);
	// Return if there's already an extension
	if (ext[0] == '.'){
		#ifdef DEBUG_ON
			printf("'appendFileExtension' - File already has an extension.\n");
		#endif
		FreeBlock(ext);
		return;
	}
	FreeBlock(ext);
	strncat(*filename, ".", 1);
	strncat(*filename, extension, stringSize);
}

FILE *openFile(String *filename, String accessMode){
	FILE *file;
	if (stringIsEmpty(*filename) || *filename[0] == '.'){
		userError(noFilename);
	}
	// Add txt extension to 'filename' if not already there
	appendFileExtension(filename, "txt");

	// Open file and check for errors
	if (!(file = fopen(*filename, accessMode))){
		userError(fileOpen, *filename, *filename);
	}
	return file;
}

void closeFile(FILE *file){
	if (fclose(file) == EOF){
		Error("Error while closing file");
	}
}

// Load file to vector
void loadWordsFromFile(String filename, Vector *pVector){
	String word;
	FILE *fileToLoad = openFile(&filename, "r");

	vectorClear(pVector);
	// Temporary memory
	word = GetBlock(MAX_WORD_LENGTH);
	// Read word into temporary memory
	for (int i = 0; fscanf(fileToLoad, "%s\n", word) != EOF; i++){
		// Append word to vector
		vectorAppend(pVector, word, (StringLength(word) + 1));
	}
	closeFile(fileToLoad);
	FreeBlock(word);
	userSucces(loadFile, filename);
	FreeBlock(filename);
	changes = 0;
}

// Save vector to file
void saveWordsToFile(String filename, Vector *pVector){
	if (vectorSize(pVector) <= 0){
		#ifdef DEBUG_ON
			printf("'saveWordsToFile' - Vector doesn't contain anything\n");
		#endif
		userError(saveEmpty, "");
	}
	FILE *fileToSave = openFile(&filename, "w");

	for (int i = 0; i < vectorSize(pVector); i++){
		fprintf(fileToSave, "%s\n", (String)vectorGet(pVector, i));
	}
	closeFile(fileToSave);
	userSucces(saveFile, filename);
	FreeBlock(filename);
	changes = 0;
}

// Print one word to the screen with its position
void printToScreen(String word, int position){
	printf("\n%d\t%s", position, word);
}

// Print all words in vector to the screen
void printWordsInVector(Vector *pVector, int startIndex, int numberOfWords){
	// Check if out of bounds
	if (startIndex < 0 || startIndex >= vectorSize(pVector)){
		#ifdef DEBUG_ON
			printf("printWordsInVector: startIndex %d is out of bounds for vector of size %d\n", startIndex, pVector->size);
		#endif
	}
	// Check if function would fetch data out of bounds of vector
	if ((startIndex + numberOfWords) > vectorSize(pVector)){
		#ifdef DEBUG_ON
			printf("printWordsInVector: startIndex %d + numberOfWords %d will print data out of bounds for vector of size %d\n", startIndex, numberOfWords, pVector->size);
		#endif
	}
	if (vectorSize(pVector) <= 0){
		userError(emptyList, "");
	}
	for (int i = startIndex; i < startIndex + numberOfWords; i++){
		printToScreen(vectorGet(pVector, i), i);
	}
}

// Find the postition for word in lexigraphical order and return position
int findPosForWord(String word, Vector *pVector){
	int i;
	word = ConvertToLowerCase(word);
	for (i = 0; i < vectorSize(pVector); i++){
		String wordInVector = ConvertToLowerCase(vectorGet(pVector, i));
		if (StringCompare(word, wordInVector) < 0){
			FreeBlock(wordInVector);
			return i;
		}
		FreeBlock(wordInVector);
	}
	return i;
}

// Find word and return word position
int getPosForWord(String word, Vector *pVector){
	if (stringIsEmpty(word)){
		#ifdef DEBUG_ON
			printf("'getPosForWord' - Word to search for is empty string\n");
		#endif
		userError(nullWord, word);
	}
	for (int i = 0; i < vectorSize(pVector); i++){
		if (stringEqualNotCaseSens(word, vectorGet(pVector, i))){
			return i; // i = position
		}
	}
	userError(noWordFound, word);
	return -1;
}

// Find words matching the 'searchTerm' and return a vector containing matching words
Vector searchForWords(String searchTerm, Vector *pVector){
	Vector resultVector;
	vectorInit(&resultVector);

	String searchTermLower = ConvertToLowerCase(searchTerm);

	for (int i = 0; i < vectorSize(pVector); i++){
		String wordInVector = ConvertToLowerCase(vectorGet(pVector, i));
		if (FindString(searchTermLower, wordInVector, 0) >= 0){
			vectorAppend(&resultVector, vectorGet(pVector, i), (StringLength(vectorGet(pVector, i)) + 1));
		}
		FreeBlock(wordInVector);
	}
	FreeBlock(searchTermLower);
	return resultVector;
}

// Find words matching the 'searchTerm' and prints them to the screen
void searchVector(String searchTerm, Vector *pVector){
	Vector wordSearch = searchForWords(searchTerm, pVector);

	if (vectorSize(&wordSearch) > 0) {
		for (int i = 0; i < vectorSize(&wordSearch); i++){
			String wordFound = vectorGet(&wordSearch, i);
			printToScreen(wordFound, getPosForWord(wordFound, pVector));
		}
		vectorFree(&wordSearch);
	}
	else{
		vectorFree(&wordSearch);
		userError(noSearchResult, searchTerm);
	}
}

void deleteWord(int index, Vector *pVector){
	// Check if out of bounds
	if (index < 0 || index >= vectorSize(pVector)){
		#ifdef DEBUG_ON
			printf("'deleteWord' - Index %d is out of bounds for vector of size %d\n", index, pVector->size);
		#endif
		userError(outOfBounds, index, (vectorSize(pVector) > 0 ? vectorSize(pVector) - 1 : 0));
	}
	userSucces(wordDelete, vectorGet(pVector, index));
	vectorRemove(pVector, index);
	changes++;
}

// TODO: Antagligen onödig, bättre och köra deleteWord() i en loop
void deleteManyWords(int index, int numWords, Vector *pVector){
	for (int i = index; i < (index + numWords); i++){
		vectorRemove(pVector, index);
	}
}

// TODO: Accept swedish chars as new word, dosen't work with åäö ÅÄÖ
// TODO: Make first char upper case
void addWord(String word, int index, Vector *pVector){
	// Check if out of bounds
	if (index < 0 || index > vectorSize(pVector)){
		#ifdef DEBUG_ON
			printf("'addWord' - Index %d is out of bounds for vector of size %d\n", index, pVector->size);
		#endif
	}
	// Error: 'word' can not contain symbol or number
	if (!stringIsAlpha(word)){
		userError(notAllowedChar, word);
	}
	// Error: 'word' can not be an empty string 
	if (stringIsEmpty(word)){
		userError(nullWord);
	}
	// Error: 'word' does already exist in vector
	if (index > 0 && stringEqualNotCaseSens(word, vectorGet(pVector, index - 1))){
		userError(alreadyExist, word);
	}
	// If we're adding word last in vector
	if (index == vectorSize(pVector)){
		vectorAppend(pVector, word, (StringLength(word) + 1));
		userSucces(wordAdded, word);
		changes++;
		return;
	}
	// If we're inserting word at 'index' in vector
	vectorInsert(pVector, index, word, (StringLength(word) + 1));
	userSucces(wordAdded, word);
	changes++;
}

void sortVector(Vector *pVector){
	int position;
	String temp;
	for (int i = 0; i < vectorSize(pVector); i++){
		for (int j = 0; j < vectorSize(pVector); j++)
		{
			if (findPosForWord(vectorGet(pVector, j), pVector) > findPosForWord(vectorGet(pVector, i), pVector)){
				temp = vectorGet(pVector, i);
				vectorSet(pVector, i, vectorGet(pVector, j));
				vectorSet(pVector, j, temp);
			}
		}
	}
}

// TODO: Accept swedish chars as new word, dosen't work with åäö ÅÄÖ
void editWord(int index, Vector *pVector){
	// Check if out of bounds
	if (index < 0 || index >= vectorSize(pVector)){
		#ifdef DEBUG_ON
			printf("'editWord' - Index %d is out of bounds for vector of size %d\n", index, pVector->size);
		#endif
		userError(outOfBounds, index, (vectorSize(pVector) > 0 ? (vectorSize(pVector)-1) : 0));
	}
	String wordToEdit = vectorGet(pVector, index);
	printf("Enter word to replace %s: ", (String)vectorGet(pVector, index));
	String newWordRaw = GetLine();
	String newWord = ConvertToLowerCase(newWordRaw);
	FreeBlock(newWordRaw);
	// Error: If 'newWord' exist in the list
	for (int i = 0; i < pVector->size; i++){
		if (stringEqualNotCaseSens(newWord, vectorGet(pVector, i))){
			userError(alreadyExist, newWord);
		}
	}
	vectorInsert(pVector, findPosForWord(newWord, pVector), newWord, (StringLength(newWord) + 1));
	userSucces(wordEdit, wordToEdit, newWord);
	vectorRemove(pVector, index);
	changes++;
}

void printHelpInfo(){
	printf("***************************************************************************\n");
	printf(" This is a list of the commands available in the program.\n With examples of how they are used, and a description explaining what they do.\n");
	printf(" All input are case-insensitive, that is \"LoAd fiLE\" works just as good as \"load file\".\n\n");
	printf(" |Command| \t|Usage| \t\t|Description|\n\n");
	printf(" load \t\tload file \t\tLoad a file into the program by specifying the filename.\n");
	printf(" save \t\tsave file \t\tSave the list to a file by specifying the filename.\n");
	printf(" clear \t\tclear \t\t\tClear the list in the program of all entries.\n");
	printf(" add \t\tadd giraffe \t\tAdd a word to the list, if the word already exist you will get an error.\n");
	printf(" edit \t\tedit giraffe \t\tEdit a word in the list by specifying the word or index of the word.\n");
	printf("\t\tedit 42 \n");
	printf(" delete \tdelete giraffe \t\tDelete a word by specifying the word or the index of the word.\n");
	printf("\t\tdelete 42 \n");
	printf(" find \t\tfind giraffe  \t\t\Find a word or part of a word, prints the result to the screen.\n");
	printf("\t\tfind a \n");
	printf(" print \t\tprint \t\t\tPrint the full list to the screen. \n");
	printf(" exit \t\texit \t\t\tExit the program, discards all unsaved changes.\n\n");
	printf("***************************************************************************");
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
	clear,
	exitProg
};

int readCommand(String command){
	if (StringEqual("help", command)){
		return help;
	}
	if (StringEqual("add", command)){
		return add;
	}
	if (StringEqual("delete", command)){
		return delete;
	}
	if (StringEqual("find", command)){
		return find;
	}
	if (StringEqual("print", command)){
		return print;
	}
	if (StringEqual("load", command)){
		return load;
	}
	if (StringEqual("save", command)){
		return save;
	}
	if (StringEqual("clear", command)){
		return clear;
	}
	if (StringEqual("edit", command)){
		return edit;
	}
	if (StringEqual("exit", command)){
		return exitProg;
	}
	return 0;
}

int commandSelection(Vector *userInput, Vector *pVector) {
	String command = vectorGet(userInput, 0);
	String value = vectorGet(userInput, 1);

	int error, position, userWantsToQuit;
	bool inputIsDigit = FALSE;

	if (!stringIsEmpty(value) && stringIsNumber(value)){
		inputIsDigit = TRUE;
		position = StringToInteger(value); // Make the string 'value' to an int
	}

	switch (readCommand(command)){

	case (help) :
		printHelpInfo();
		return 1;

	case (add) :
		error = setjmp(env);
		if (error == 0){
			addWord(value, findPosForWord(value, pVector), pVector);
		}
		return 1;

	case (delete) :
		error = setjmp(env);
		if (error == 0){
			if (inputIsDigit){
				deleteWord(position, pVector);
			}
			else{
				deleteWord(getPosForWord(value, pVector), pVector);
			}
		}
		return 1;

	case (edit) :
		error = setjmp(env);
		if (error == 0){	
			if (inputIsDigit){
				editWord(position, pVector);
			}
			else{
				editWord(getPosForWord(value, pVector), pVector);
			}
		}
		return 1;

	case (find) :
		error = setjmp(env);
		if (error == 0){
			searchVector(value, pVector);
		}
		return 1;

	case (print) :
		printWordsInVector(pVector, 0, vectorSize(pVector));
		return 1;

	case (load) :
		error = setjmp(env);
		if (error == 0){
			loadWordsFromFile(value, pVector);
		}
		return 1;

	case (save) :
		error = setjmp(env);
		if (error == 0){
			saveWordsToFile(value, pVector);
		}
		return 1;

	case (clear) :
		vectorClear(pVector);
		userSucces(clearList);
		changes = 0;
		return 1;

	case (exitProg) :
		userWantsToQuit = checkSaveChanges(userInput);
		if (userWantsToQuit){
			return 0;
		}
		return 1;

	default:
		userError(unknownCommand, command);
		return 1;
	}
}

void getInput(Vector *userInput){
	String	userInputRaw,
			userInputLower,
			commandInput,
			valueInput;

	vectorClear(userInput);												// Clear vector of old input
	printf("\n\n%c", '>');
	userInputRaw = GetLine();											// Read input from user

	if (StringLength(userInputRaw) > MAX_WORD_LENGTH){					// Make sure input is not to long (arbitrary limit)
		userError(toLongInput, "");
	}
	userInputLower = ConvertToLowerCase(userInputRaw);					// Convert to lower case
	FreeBlock(userInputRaw);

	int spaceChar = (FindChar(' ', userInputLower, 0));					// Find space character to seperate command and value inputs
	if (spaceChar == -1){												// Only command entered
		commandInput = SubString(userInputLower, 0, StringLength(userInputLower));
		valueInput = GetBlock(1);
		valueInput[0] = '\0';
	}
	else{																// Both command and value entered
		commandInput = SubString(userInputLower, 0, (spaceChar - 1));
		valueInput = SubString(userInputLower, (spaceChar + 1), StringLength(userInputLower));
	}
	convertToSweString(valueInput);										// Convert input to swedish characters

	vectorAppend(userInput, commandInput, (StringLength(commandInput) + 1));	// Save input in vector
	vectorAppend(userInput, valueInput, (StringLength(valueInput) + 1));
	FreeBlock(commandInput);
	FreeBlock(valueInput);
}

int checkSaveChanges(Vector *pVector){
	if (changes){
		userError(unsavedChanges, "");
		int error = setjmp(env);
		if (error == 0){
			getInput(pVector);
			if (readCommand(vectorGet(pVector, 0)) == exitProg){
				return 1;
			}
		}
		userSucces(continueProg);
		return 0;
	}
}
//###########################################################//
void main()
{
	if (!setlocale(LC_ALL, "")) {
		printf("error while setting locale\n");
	}

	Vector userInput;
	vectorInit(&userInput);
	Vector container;
	vectorInit(&container);

	//loadWordsFromFile("ordlista.txt", &container);

	printf("WordMagic ver 0.1\n");
	printf("To get started, type help.");

	int check = 1;
	while (check){
		int error = setjmp(env);
		if (error == 0){
			getInput(&userInput);
			check = commandSelection(&userInput, &container);
		}
	}
}
