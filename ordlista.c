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
	int size;		// slots used so far
	int capacity;	// total available slots
	void **data;	// array of integers we're storing
} Vector;


//###########################################################//
void vectorInit(Vector *pVector) {
	// Initialize size and capacity
	pVector->size = 0;
	pVector->capacity = VECTOR_INITIAL_CAPACITY;

	// Allocate memory for vector->data
	pVector->data = GetBlock(sizeof(void*)* pVector->capacity);
}

int vectorSize(Vector *pVector)
{
	return pVector->size;
}

static void vectorDoubleCapacityIfFull(Vector *pVector) {
#ifdef DEBUG_ON
	printf("Vector resize: %d to %d\n", pVector->size, pVector->capacity);
#endif

	if (pVector->size >= pVector->capacity) {

		void **newMemory = realloc(pVector->data, sizeof(void*)* (pVector->capacity * 2));
		if (newMemory) {
			pVector->data = newMemory;
			pVector->capacity *= 2;
		}
	}
}

static void vectorHalfCapacityIfNotUsed(Vector *pVector) {
#ifdef DEBUG_ON
	printf("Vector resize: %d to %d\n", pVector->capacity, pVector->size);
#endif

	if (pVector->capacity / pVector->size >= 2) {

		void **newMemory = realloc(pVector->data, sizeof(void*)* (pVector->capacity / 2));
		if (newMemory) {
			pVector->data = newMemory;
			pVector->capacity /= 2;
		}
	}
}

void vectorAppend(Vector *pVector, void **value, int sizeOfElem) {
	// Make sure there's room to expand into
	vectorDoubleCapacityIfFull(pVector);

	// Append the value and increment vector->size
	void* ptr = malloc(sizeOfElem);
	memmove(ptr, value, sizeOfElem);
	pVector->data[pVector->size] = ptr;
	pVector->size++;
}

void vectorSet(Vector *pVector, int index, void **value) {

	if (index >= pVector->size || index < 0) {
		printf("'vectorSet' - Index %d is out of bounds for vector of size %d\n", index, pVector->size);
		//exit(1);
	}
	// Set the value at the desired index
	pVector->data[index] = value;
}

void *vectorGet(Vector *pVector, int index) {
	if (index >= pVector->size || index < 0) {
		printf("'vectorGet' - Index %d is out of bounds for vector of size %d\n", index, pVector->size);
		//exit(1);
	}
	return pVector->data[index];
}

//
// Moves all values by one starting at index and inserts new value at index
//
void vectorInsert(Vector *pVector, int index, void *value) {
	// Make sure there's room to expand into
	vectorDoubleCapacityIfFull(pVector);

	for (int i = pVector->size; i > index; i--) {
		vectorSet(pVector, i, pVector->data[i - 1]);
	}

	vectorSet(pVector, index, value);
}

void vectorRemove(Vector *pVector, int index) {

	for (int i = index; i < (pVector->size - 1); i++) {
		vectorSet(pVector, i, pVector->data[i + 1]);
	}
	vectorSet(pVector, (pVector->size-1), NULL);
	pVector->size--;

	vectorHalfCapacityIfNotUsed(pVector);
}

void vectorFree(Vector *pVector)
{
	free(pVector->data);
}

//###########################################################//
void storeWordsFromFile(String filename, Vector *pVector) {
	FILE *file;
	String word;

	// Open file and check for errors
	if (!(file = fopen("Ordlista.txt", "r"))) {
		printf("Error opening %s\n", "Ordlista.txt");
		return;
	}

	word = malloc(MAX_WORD_LENGTH);
	int five = 5;
	// Read word into temporary memory

	for (int i = 0; fscanf(file, "%s\n", word) != EOF; i++) {
		Vector a;
		vectorInit(&a);
		// Add word to vector pVectorToAdd
		vectorAppend(&a, word, (strlen(word) + 1));
		// Save vector pVectorToAdd to vector pVector

		vectorAppend(pVector, &a, sizeof(Vector));
	}
	free(word);
	fclose(file);
}

int compareWords(String wordA, String wordB) {
	return memcmp(wordA, wordB, strlen(wordA));
}

//Find word and return word position
int findPosForWord(String word, Vector *pVector) {
	for (int i = 0; i < vectorSize(pVector); i++){
		//Check if strings match with memcmp
		Vector *compare = vectorGet(pVector, i);
		if (memcmp(word, vectorGet(compare, 0), strlen(word)) == 0){
			return i; // i = position
		}
	}
	return -1;
}

int* searchForWords(String searchTerm, Vector *pVector) {

	int *pCompareVector = calloc(pVector->size, sizeof(int));

	for (int i = 0; i < vectorSize(pVector); i++) {
		Vector *word = vectorGet(pVector, i);
		if (strstr(vectorGet(word, 0), searchTerm) != NULL) {
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
	if (index < 0 || index >= vectorSize(pVector)) {
		printf("Index %d is out of bounds for vector of size %d\n", index, pVector->size);
		return -1;
	}

	vectorInsert(pVector, index, &word);
	printf("Ordet la tills\n");
	return 1;
}

// Behaves wierdly after its done printing every value in vector
void printWordsInVector(Vector *pVector, int startIndex, int endIndex) {
	if (startIndex < vectorSize(pVector) && startIndex < endIndex)
	{
		Vector *word = vectorGet(pVector, startIndex);
		printf("\n%d\t%s", startIndex, vectorGet(word, 0));
		printWordsInVector(pVector, ++startIndex, endIndex);
	}
}

int editWord(int index, Vector *pVector)
{
	Vector *wordToEdit = vectorGet(pVector, index);

	String word = GetBlock(MAX_WORD_LENGTH);
	printf("The word you are about to edit: %s.\n", *(wordToEdit->data));
	printf("\tDO IT,JUST DO IT!\nDont let your dreams be dreams, edit %s now: ",*(wordToEdit->data)); //for the lol's. 
	scanf("%s", word);

	vectorSet(wordToEdit, 0, word, sizeof(strlen(word) + 1));
	vectorSet(pVector, index, wordToEdit, sizeof(Vector));

	FreeBlock(word);

	

	/*String word = GetBlock(MAX_WORD_LENGTH);

	Vector *hfhfh = vectorGet(&container, findPosForWord(value, &container));
	scanf("%s", &word);

	vectorSet(hfhfh, 0, word, (sizeof(strlen(word) + 1)));

	vectorSet(pVector, index, hfhfh, sizeof(Vector));

	FreeBlock(word);
	*/
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

typedef enum { h = (int)'h', a = (int)'a', d = (int)'d', f = (int)'f', p = (int)'p', e = (int)'e' };

int readCommand(String value) {
	if (*value == h && StringEqual("help", ConvertToLowerCase(value))) {
		return 1;
	}
	if (*value == a && StringEqual("add", ConvertToLowerCase(value))) {
		return 2;
	}
	if (*value == d && StringEqual("delete", ConvertToLowerCase(value))) {
		return 4;
	}
	if (*value == f && StringEqual("find", ConvertToLowerCase(value))) {
		return 5;
	}
	if (*value == p && StringEqual("print", ConvertToLowerCase(value))) {
		return 6;
	}
	if (*value == e) {
		if (StringEqual("edit", ConvertToLowerCase(value))) {
			return 3;
		}
		if (*value == e && StringEqual("exit", ConvertToLowerCase(value))) {
			return 7;
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
	if (spaceChar == -1) {
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
	switch (readCommand(command)) {
	case (help) :
		//printHelpInfo();
		break;
	case (add) :
		addWord(value, 10, pVector);
		return 1;
		break;
	case (delete) :
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
	case (edit) :
		editWord(findPosForWord(value, pVector), pVector);
		return 1;
		break;
	case (find) :
	{
					int *pCompareVector = searchForWords(value, pVector);
					for (int i = 0; i < pVector->size; i++) {
						if (pCompareVector[i] > 0) {
							printWordsInVector(pVector, i, (i + 1));
						}
					}
					free(pCompareVector);
					return 1;
					break;
	}
	case (print) :
	{
					 printWordsInVector(pVector, 0, pVector->size);
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
	/*Vector *test = (container.data[5]);
	int x = findPosForWord(*(test->data), &container);
	int y = findPosForWord("Butan", &container);*/

	//vectorSet(&vector, 25, "-hej-");
	//vectorSet(&vector, 89, "Coca-Cola");

	//vectorRemove(&vector, 87);

	/*vector a;
	vector_init(&a);
	String asd = "asd";
	vector_add(&a, asd);
	vector_add(&v, &a);*/

	for (int i = 0; i < vectorSize(&container); i++) {
		Vector *test = vectorGet(&container, i);
		printf("%s \n", vectorGet(test, 0));
	}


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
		check = switchCommand(command, value, &container);
	}


	//printf("findPosForWord returned: %d for word \"Brasa\".\n\n", findPosForWord("Brasa", &vector));

	//vectorRemove(&vector, 5);

	//printf("%s", addWord("--test addWord--", 45, &vector));


	//Word a = vectorGet(&vector, 29);
	//printf("\nvectorGet returned: %s", a.word);
}
