#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "genlib.h"
#include "vector.h"
#include "messer.h"
#include "listfunc.h"
#include "fileio.h"
#include "strlib.h"
#include "strmanip.h"
#include "simpio.h"

#define MAX_WORD_LENGTH 100


void loadWordsFromFile(String filename, Vector *pVector){
	String word;
	setExtension("txt");
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
}

void saveWordsToFile(String filename, Vector *pVector){
	if (vectorSize(pVector) <= 0){
		#ifdef DEBUG_ON
			printf("'saveWordsToFile' - Vector doesn't contain anything\n");
		#endif
		userError(saveEmpty);
	}
	setExtension("txt");
	FILE *fileToSave = openFile(&filename, "w");

	for (int i = 0; i < vectorSize(pVector); i++){
		fprintf(fileToSave, "%s\n", (String)vectorGet(pVector, i));
	}
	closeFile(fileToSave);
	userSuccess(saveFile, filename);
}

void printToScreen(String word, int position){
	printf("\n%d\t%s", position, word);
}

void printWordsInVector(Vector *pVector, int startIndex, int numberOfWords){
	#ifdef DEBUG_ON
		// Check if out of bounds
		if (startIndex < 0 || startIndex >= vectorSize(pVector)){
				printf("printWordsInVector: startIndex %d is out of bounds for vector of size %d\n", startIndex, pVector->size);
		}
		// Check if function would fetch data out of bounds of vector
		if ((startIndex + numberOfWords) > vectorSize(pVector)){
				printf("printWordsInVector: startIndex %d + numberOfWords %d will print data out of bounds for vector of size %d\n", startIndex, numberOfWords, pVector->size);
		}
	#endif

	if (vectorSize(pVector) <= 0){
		userError(emptyList);
	}
	for (int i = startIndex; i < startIndex + numberOfWords; i++){
		printToScreen(vectorGet(pVector, i), i);
	}
}

int findPosForNewWord(String word, Vector *pVector){
	if (stringIsEmpty(word)){
		#ifdef DEBUG_ON
			printf("'getPosForWord' - Word to search for is empty string\n");
		#endif
		userError(nullWord, word);
	}
	String compareString = CopyString(word);
	stringFormat(compareString);
	compareSwedish(compareString);

	int i;
	for (i = 0; i < vectorSize(pVector); i++){
		String wordInVector = CopyString(vectorGet(pVector, i));
		compareSwedish(wordInVector);
		if (StringCompare(compareString, wordInVector) < 0){
			FreeBlock(wordInVector);
			FreeBlock(compareString);
			return i;
		}
		FreeBlock(wordInVector);
	}
	FreeBlock(compareString);
	return i;
}

int getPosForWord(String word, Vector *pVector){
	if (stringIsEmpty(word)){
		#ifdef DEBUG_ON
			printf("'getPosForWord' - Word to search for is empty string\n");
		#endif
		userError(nullWord, word);
	}
	stringFormat(word);
	for (int i = 0; i < vectorSize(pVector); i++){
		if (StringEqual(word, vectorGet(pVector, i))){
			return i; // i = position
		}
	}
	return -1;
}

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
	// Error: out of bounds
	if (index < 0 || index >= vectorSize(pVector)){
		#ifdef DEBUG_ON
			printf("'deleteWord' - Index %d is out of bounds for vector of size %d\n", index, pVector->size);
		#endif
		userError(outOfBounds, index, (vectorSize(pVector) > 0 ? vectorSize(pVector) - 1 : 0));
	}
	userSuccess(wordDelete, vectorGet(pVector, index));
	vectorRemove(pVector, index);
}

void addWord(String word, int index, Vector *pVector){
	#ifdef DEBUG_ON
		// Check if out of bounds
		if (index < 0 || index > vectorSize(pVector)){
				printf("'addWord' - Index %d is out of bounds for vector of size %d\n", index, pVector->size);
		}
	#endif
	stringFormat(word);
	// Error: 'word' can not contain symbol or number
	if (!stringIsAlpha(word)){
		userError(notAllowedChar, word);
	}
	// Error: 'word' can not be an empty string
	if (stringIsEmpty(word)){
		userError(nullWord);
	}
	// Error: 'word' does already exist in vector
	if (index > 0 && StringEqual(word, vectorGet(pVector, index - 1))){
		userError(alreadyExist, word);
	}
	// If we're adding 'word' last in vector
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

void editWord(int index, Vector *pVector){
	// Check if out of bounds
	if (index < 0 || index >= vectorSize(pVector)){
		#ifdef DEBUG_ON
			printf("'editWord' - Index %d is out of bounds for vector of size %d\n", index, pVector->size);
		#endif
		userError(outOfBounds, index, (vectorSize(pVector) > 0 ? (vectorSize(pVector) - 1) : 0));
	}
	String wordToEdit;
	wordToEdit = vectorGet(pVector, index);
	printf("Enter word to replace %s: ", (String)vectorGet(pVector, index));

	String newWord = GetLine();
	stringFormat(newWord);
	// Error: If 'newWord' exist in the vector
	if (getPosForWord(newWord, pVector) != -1){
		userError(alreadyExist, newWord);
	}
	userSuccess(wordEdit, wordToEdit, newWord);
	vectorRemove(pVector, index);
	vectorInsert(pVector, findPosForNewWord(newWord, pVector), newWord, (StringLength(newWord) + 1));
	FreeBlock(newWord);
}

void printHelpInfo(){
	printf("***************************************************************************\n");
	printf(" This is a list of the commands available in the program.\n With examples of how they are used, and a description explaining what they do.\n");
	printf(" All input are case-insensitive, that is \"LoAd fiLE\" is interpreted the same as \"load file\".\n\n");
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