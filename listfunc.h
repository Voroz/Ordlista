#ifndef _listfunc_h
#define _listfunc_h

#include "genlib.h"
#include "vector.h"

/*
* Function: loadWordsFromFile
* Usage: loadWordsFromFile(filename, pVector);
* -------------------------------------
* Loads the file specified in 'filename' into the vector.
*/
void loadWordsFromFile(String filename, Vector *pVector);

/*
* Function: saveWordsToFile
* Usage: saveWordsToFile(filename, pVector);
* -------------------------------------
* Saves the vector to the file specified in 'filename'.
*/
void saveWordsToFile(String filename, Vector *pVector);

/*
* Function: printToScreen
* Usage: printToScreen(word, position);
* -------------------------------------
* Prints one word to the screen with its position.
*/
void printToScreen(String word, int position);

/*
* Function: printWordsInVector
* Usage: printWordsInVector(pVector, startIndex, numberOfWords);
* -------------------------------------
* Print words in vector to the screen, starting at 'startindex',
* and prints as many words specified in 'numberOfWords'.
*/
void printWordsInVector(Vector *pVector, int startIndex, int numberOfWords);

/*
* Function: findPosForNewWord
* Usage: int position = findPosForNewWord(word, pVector);
* -------------------------------------
* Find the postition for 'word' in lexigraphical order in vector and return position.
*/
int findPosForNewWord(String word, Vector *pVector);

/*
* Function: getPosForWord
* Usage: int position = getPosForWord(word, pVector);
* -------------------------------------
* Find word in vector and return word position.
* If not found returns -1 (negative one).
*/
int getPosForWord(String word, Vector *pVector);

/*
* Function: searchForWords
* Usage: Vector result = searchForWords(searchTerm, pVector);
* -------------------------------------
* Find words matching the 'searchTerm' and return a vector containing matching words.
*/
Vector searchForWords(String searchTerm, Vector *pVector);

/*
* Function: searchVector
* Usage: searchVector(searchTerm, pVector);
* -------------------------------------
* Find words matching the 'searchTerm' and prints them to the screen.
*/
void searchVector(String searchTerm, Vector *pVector);

/*
* Function: deleteWord
* Usage: deleteWord(index, pVector);
* -------------------------------------
* Deletes the word at position 'index'.
*/
void deleteWord(int index, Vector *pVector);

/*
* Function: addWord
* Usage: addWord(word, index, pVector);
* -------------------------------------
* Adds 'word' att position 'index' in vector.
*/
void addWord(String word, int index, Vector *pVector);

/*
* Function: editWord
* Usage: editWord(index, pVector);
* -------------------------------------
* Edits the word at position index.
*/
void editWord(int index, Vector *pVector);

/*
* Function: printHelpInfo
* Usage: printHelpInfo();
* -------------------------------------
* Prints information related to the appliction.
*/
void printHelpInfo();

#endif