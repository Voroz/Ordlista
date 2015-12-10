#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <windows.h>
#include "genlib.h"
#include "random.h"
#include "simpio.h"
#include "strlib.h"

#include "fileio.h"
#include "listfunc.h"
#include "strmanip.h"
#include "ui.h"
#include "user.h"
#include "vector.h"
#include "errorhand.h"

/*
#define VECTOR_INITIAL_CAPACITY 10
#define MAX_WORD_LENGTH 100
//#define DEBUG_ON

typedef struct{
	int size;		// Slots used so far
	int capacity;	// Total available slots
	void **data;	// Array of data we're storing
} Vector;


// TODO: New function which format string to lowercase with first char as upper case letter

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
}

void vectorSet(Vector *pVector, int index, void* *value){
	if (index >= pVector->size || index < 0){
		#ifdef DEBUG_ON
			printf("'vectorSet' - Index %d is out of bounds for vector of size %d\n", index, pVector->size);
		#endif
	}
	else{
		// Set the value at the desired index
		pVector->data[index] = value;
	}
}

void* vectorGet(Vector *pVector, int index){
	if (index >= pVector->size || index < 0){
		#ifdef DEBUG_ON
			printf("'vectorGet' - Index %d is out of bounds for vector of size %d\n", index, pVector->size);
		#endif
		return NULL; //##########// TODO: Testa!
	}
	else{
		return pVector->data[index];
	}
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
	else{
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
}

void vectorRemove(Vector *pVector, int index){
	// Check if out of bounds
	if (index < 0 || index >= pVector->size){
		#ifdef DEBUG_ON
			printf("'vectorRemove' - Index %d is out of bounds for vector of size %d\n", index, pVector->size);
		#endif
	}
	else{
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
static jmp_buf *env;

errorSetEnv(jmp_buf *currentEnv){
	env = currentEnv;
}

typedef enum{
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
} ErrorCode;

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
		vfprintf(stderr, "\nYou have changes that are not saved \n- Type %s to discard changes \n- Press enter to return to program", args);
		break;
	case (unknownCommand) :
		vfprintf(stderr, "\nUnknown command", args);
		break;
	default:
		break;
	}
	va_end(args);
}

typedef enum{
	wordAdded,
	wordDelete,
	wordEdit,
	loadFile,
	saveFile,
	clearList,
	continueProg
} SuccessCode;

void userSuccess(SuccessCode succes, ...){
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
		if (!(*s >= 'A' && *s <= 'Z') && !(*s >= 'a' && *s <= 'z') && !(*s >= 'ä' && *s <= 'ö'))
			return FALSE;
		s++;
	}
	return TRUE;
}

String findExtension(String filename){
	return SubString(filename, FindCharFromRight('.', filename, 0), StringLength(filename));
}

// TODO: Try to make the memory situation better
// 'strcat' destination needs to be large enough to hold the new string
// Problem starts in 'openFile' with 'appendFileExtension'
void appendFileExtension(String *filename, String extension){
	// Return if there's already an extension
	if (findExtension(*filename)[0] == '.'){
		#ifdef DEBUG_ON
			printf("'appendFileExtension' - File already has an extension.\n");
		#endif
	}
	else{
		*filename = Concat(*filename, ".");
		*filename = Concat(*filename, extension);
	}
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
		userError(fileOpen, *filename);
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
	userSuccess(loadFile, filename);
	FreeBlock(filename);
}

// Save vector to file
void saveWordsToFile(String filename, Vector *pVector){
	if (vectorSize(pVector) <= 0){
		#ifdef DEBUG_ON
			printf("'saveWordsToFile' - Vector doesn't contain anything\n");
		#endif
		userError(saveEmpty);
	}
	FILE *fileToSave = openFile(&filename, "w");

	for (int i = 0; i < vectorSize(pVector); i++){
		fprintf(fileToSave, "%s\n", (String)vectorGet(pVector, i));
	}
	closeFile(fileToSave);
	userSuccess(saveFile, filename);
	FreeBlock(filename);
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
		userError(emptyList);
	}
	for (int i = startIndex; i < startIndex + numberOfWords; i++){
		printToScreen(vectorGet(pVector, i), i);
	}
}

// Find the postition for word in lexigraphical order and return position
int findPosForWord(String word, Vector *pVector){
	int i;
	word = ConvertToLowerCase(word);
	
	for (int chr = 0; chr < StringLength(word); chr++){
		if (word[chr] == 'ä' || word[chr] == 'Ä'){
			word[chr] += 2;
		}
	}
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
			String wordFound = 0;
			wordFound = vectorGet(&wordSearch, i);
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
	// Error: if out of bounds
	if (index < 0 || index >= vectorSize(pVector)){
		#ifdef DEBUG_ON
			printf("'deleteWord' - Index %d is out of bounds for vector of size %d\n", index, pVector->size);
		#endif
		userError(outOfBounds, index, (vectorSize(pVector) > 0 ? vectorSize(pVector) - 1 : 0));
	}
	userSuccess(wordDelete, vectorGet(pVector, index));
	vectorRemove(pVector, index);
}

// TODO: Accept swedish chars as new word, dosen't work with åäö ÅÄÖ
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

	word[0] = toupper(word[0]);
	// If we're adding word last in vector
	if (index == vectorSize(pVector)){
		vectorAppend(pVector, word, (StringLength(word) + 1));
		userSuccess(wordAdded, word);
	}
	// If we're inserting word at 'index' in vector
	else{
		vectorInsert(pVector, index, word, (StringLength(word) + 1));
		userSuccess(wordAdded, word);
	}
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
	String wordToEdit = 0;
	wordToEdit = vectorGet(pVector, index);
	printf("Enter word to replace %s: ", (String)vectorGet(pVector, index));
	String newWordRaw = GetLine();
	String newWord = ConvertToLowerCase(newWordRaw);
	FreeBlock(newWordRaw);
	// Error: If 'newWord' exist in the vector
	for (int i = 0; i < vectorSize(pVector); i++){
		if (getPosForWord(newWord, pVector) != -1){
			userError(alreadyExist, newWord);
		}
	}
	userSuccess(wordEdit, wordToEdit, newWord);
	vectorRemove(pVector, index);
	vectorInsert(pVector, findPosForWord(newWord, pVector), newWord, (StringLength(newWord) + 1));
	FreeBlock(wordToEdit);
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
	printf(" find \t\tfind giraffe  \t\tFind a word or part of a word, prints the result to the screen.\n");
	printf("\t\tfind a \n");
	printf(" print \t\tprint \t\t\tPrint the full list to the screen. \n");
	printf(" exit \t\texit \t\t\tExit the program, discards all unsaved changes.\n\n");
	printf("***************************************************************************");
}

//###########################################################//
typedef enum commands{
	help = 1,
	add,
	edit,
	deelete,
	find,
	print,
	load,
	save,
	clear,
	exitProg
} Command;

typedef struct{
	String filename;
	Bool modified;
} GlobalState;

typedef struct{
	Command command;
	Vector input;
	GlobalState globals;
} User;

typedef enum{
	string,
	digit
} InputType;

static InputType inType;
static int error;

void userNewUser(User *pUserInput){
	Vector input;
	vectorInit(&input);
	pUserInput->input = input;
	pUserInput->globals.filename = "";
	pUserInput->globals.modified = FALSE;
}
void userSetCom(Command com, User *pUserInput){
	pUserInput->command = com;
}
Command userGetCom(User *pUserInput){
	return pUserInput->command;
}
void userAddInput(void* input, int sizeOfElem, User *pUserInput){
	if (inType){
		vectorAppend(&pUserInput->input, &input, sizeOfElem);
	}
	else{
		vectorAppend(&pUserInput->input, input, sizeOfElem);
	}
}
Vector* userGetInput(User *pUserInput){
	return &pUserInput->input;
}
void userSetFilename(String filename, User *pUserInput){
	String str = GetBlock(StringLength(filename) + 1);
	memcpy(str, filename, (StringLength(filename) + 1));
	if (!stringIsEmpty(pUserInput->globals.filename)){
		FreeBlock(pUserInput->globals.filename);
	}
	pUserInput->globals.filename = str;
}
String userGetFilename(User *pUserInput){
	return pUserInput->globals.filename;
}
void userSetMod(int n, User *pUserInput){
	pUserInput->globals.modified = n;
}
Bool userGetMod(User *pUserInput){
	return pUserInput->globals.modified;
}
void userSetEnv(int n, jmp_buf *env){
	error = n;
	errorSetEnv(env);
}
int userGetNoError(){
	return (error == 0);
}
void userSetInType(InputType t){
	inType = t;
}
Bool userGetInType(){
	return inType;
}

int commandSelection(User *pUserInput, Vector *pContainer) {

	Command command = userGetCom(pUserInput);
	Bool inputIsDigit = userGetInType(pUserInput);
	void **input;
	int userWantsToQuit, userWantsToLoad;

	if (inputIsDigit){
		input = (int*)vectorGet(userGetInput(pUserInput), 0);
	}
	else{
		input = (String)vectorGet(userGetInput(pUserInput), 0);
	}
		
	switch (command){

		case (help) :
			printHelpInfo();
			return 1;

		case (add) :
			addWord(input, findPosForWord(input, pContainer), pContainer);
			userSetMod(1, pUserInput);
			return 1;

		case (deelete) :
			if (inputIsDigit){
				deleteWord(*input, pContainer);
			}
			else{
				deleteWord(getPosForWord(input, pContainer), pContainer);
			}
			userSetMod(1, pUserInput);
			return 1;

		case (edit) :
			if (inputIsDigit){
				editWord(*input, pContainer);
			}
			else{
				editWord(getPosForWord(input, pContainer), pContainer);
			}
			userSetMod(1, pUserInput);
			return 1;

		case (find) :
			searchVector(input, pContainer);
			return 1;

		case (print) :
			printWordsInVector(pContainer, 0, vectorSize(pContainer));
			return 1;

		case (load) :
			userWantsToLoad = checkSaveChanges(pUserInput, load);
			if (userWantsToLoad){
				loadWordsFromFile(input, pContainer);
				userSetMod(0, pUserInput);
			}
			return 1;

		case (save) :
			if (!stringIsEmpty(userGetFilename(pUserInput))){
				saveWordsToFile(userGetFilename(pUserInput), pContainer); // Save to last active file
			}
			else{
				saveWordsToFile(input, pContainer); // Save to file specified in 'input'
			}
			userSetMod(0, pUserInput);
			return 1;

		case (clear) :
			vectorClear(pContainer);
			userSuccess(clearList);
			return 1;

		case (exitProg) :
			userWantsToQuit = checkSaveChanges(pUserInput, deelete);
			if (userWantsToQuit){
				return 0;
			}
			return 1;

		default:
			userError(unknownCommand);
			return 1;
	}
}

int readCommand(String command){
	if (StringEqual("help", command)){
		return help;
	}
	if (StringEqual("add", command)){
		return add;
	}
	if (StringEqual("delete", command)){
		return deelete;
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

// TODO: Use not yet made function to format 'input1', 'input2'
void getInput(User *pUserInput){
	String	userInputRaw,
			command = GetBlock(MAX_WORD_LENGTH),
			input1 = GetBlock(MAX_WORD_LENGTH),
			input2 = GetBlock(MAX_WORD_LENGTH);

	userInputRaw = GetLine(); // Get input string
	
	if (StringLength(userInputRaw) > MAX_WORD_LENGTH){ // Make sure input is not to long (arbitrary limit)
		userError(toLongInput);
	}
	int numberOfArgs = sscanf(userInputRaw, "%s %s %s", command, input1, input2); // Format input string, get amount of inputs

	vectorClear(userGetInput(pUserInput));
	Command com = readCommand(command);
	userSetCom(com, pUserInput);
	
	if (numberOfArgs > 1){
		convertToSweString(input1);
		if (stringIsNumber(input1) && com == deelete || com == edit){
			userSetInType(digit);
			int number = StringToInteger(input1); // Make the string 'input' to an int
			userAddInput(number, pUserInput, sizeof(int));
		}
		else{
			userSetInType(string);
			userAddInput(input1, (StringLength(input1) + 1), pUserInput);
			if (numberOfArgs > 2){
				convertToSweString(input2);
				userAddInput(input2, (StringLength(input2) + 1), pUserInput);
			}
		}
		if (com == load || com == save){
			userSetFilename(input1, pUserInput);
		}
	}
	FreeBlock(userInputRaw);
	FreeBlock(command);
	FreeBlock(input1);
	FreeBlock(input2);
}

Bool checkSaveChanges(User *pUserInput, Command com){
	if (userGetMod(pUserInput)){
		jmp_buf envSaveChanges;
		userSetEnv(setjmp(envSaveChanges), &envSaveChanges);
		if (userGetNoError()){
			if (com == exitProg){
				userError(unsavedChanges, "exit");
			}
			else{
				userError(unsavedChanges, "load");
			}
			getInput(pUserInput);
			if (userGetCom(pUserInput) == exitProg || userGetCom(pUserInput) == load){
				return TRUE;
			}
		}
		userSuccess(continueProg);
		return FALSE;
	}
	return TRUE;
}
###########################################################//
*/

int main()
{
	COORD buffer = { 114, 800 };
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), buffer);
	SMALL_RECT winRect = { 0, 0, 122, 30 };
	SMALL_RECT* winSize = &winRect;
	SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), 1, winSize);
	//system("MODE CON: COLS=140 LINES=9000"); //MODE CON[:] [COLS=c] [LINES=n]

	if (!setlocale(LC_ALL, "")) {
		printf("error while setting locale\n");
	}

	Vector container;
	vectorInit(&container);
	User userInput;
	userNewUser(&userInput);

	printf("WordMagic ver 1.0\nTo get started, type help.");

	int check = 1;
	while (check){
		jmp_buf envMain;
		userSetEnv(setjmp(envMain), &envMain);
		if (userGetNoError()){
			getInput(&userInput);
			check = commandSelection(&userInput, &container);
		}
	}
	return 0;
}