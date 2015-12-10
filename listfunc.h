#ifndef _listfunc_h
#define _listfunc_h

#include "genlib.h"
#include "vector.h"

void loadWordsFromFile(String filename, Vector *pVector);

// Save vector to file
void saveWordsToFile(String filename, Vector *pVector);

// Print one word to the screen with its position
void printToScreen(String word, int position);

// Print all words in vector to the screen
void printWordsInVector(Vector *pVector, int startIndex, int numberOfWords);

// Find the postition for word in lexigraphical order and return position
int findPosForWord(String word, Vector *pVector);

// Find word and return word position
int getPosForWord(String word, Vector *pVector);

// Find words matching the 'searchTerm' and return a vector containing matching words
Vector searchForWords(String searchTerm, Vector *pVector);

// Find words matching the 'searchTerm' and prints them to the screen
void searchVector(String searchTerm, Vector *pVector);

void deleteWord(int index, Vector *pVector);

// TODO: Accept swedish chars as new word, dosen't work with åäö ÅÄÖ
void addWord(String word, int index, Vector *pVector);

void sortVector(Vector *pVector);

// TODO: Accept swedish chars as new word, dosen't work with åäö ÅÄÖ
void editWord(int index, Vector *pVector);

void printHelpInfo();

#endif