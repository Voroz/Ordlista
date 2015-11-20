#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

const int maxWordLength = 100;

typedef struct{
	char* string;
	int size;
}Words;

Words* storeFileWords(char* filename){
	//Read from file
	FILE *file;
	if (!(file = fopen(filename, "r"))){
		printf("Error opening %s\n", filename);
		return;
	}

	//Read size and allocate memory
	Words *words;
	int arraySize = 0;
	fscanf(file, "%d //Amount of words\n", &arraySize);
	words = (Words*)malloc(arraySize*sizeof(Words));
	//First word contains size of array
	words[0].size = arraySize;

	//Allocate memory for string length and save in array
	words[1].string = (char*)malloc(maxWordLength * sizeof(char)); //Temp memory
	for (int i = 1; fscanf(file, "%s\n", words[i].string) != EOF; i++){
		words[i].size = strlen(words[i].string);
		//Reallocate memory to match word size
		printf("%d\n", strlen(words[i].string));
		words[i].string = (char*)realloc(words[i].string, words[i].size * sizeof(char*));          // Behövs?, Varför char*?
		printf("%d\n", strlen(words[i].string));
		//Allocate room for next word (unless we are at last word)
		if (i < arraySize - 1){
			words[i + 1].string = (char*)malloc(maxWordLength * sizeof(char)); //Temp memory
		}
	}

	fclose(file);
	return words;
}

void addWord(char* word, int position){

}

//Find word and return word position
int findWord(char* word, Words *words){
	for (int i = 1; i < words[0].size; i++){
		//Check if strings match with memcmp
		if (memcmp(word, words[i].string, words[i].size) == 0){
			return i; // i = position
		}
	}
	return 0;
}


Words* moveDownWords(int firstWordPos, int amountOfWords, int distance, Words *words){
	int sizeDifference = firstWordPos + amountOfWords + distance - words[0].size,
		originalSize = words[0].size;

	if (sizeDifference > 0)
	{
		words[0].size += sizeDifference;
		words = (Words*)realloc(words, (words[0].size)*sizeof(Words));

		for (int i = originalSize; i < words[0].size - 1; i++)
		{
			words[i].string = (char*)malloc(words[i].string, maxWordLength*sizeof(char*));
		}
	}
	memmove(words + firstWordPos + distance, words + firstWordPos, amountOfWords*sizeof(Words));
	return words;
}


int main(){

	Words *words;

	words = storeFileWords("ordlista.txt");
	printf("%d\n", findWord("Allrum", words));

	words = moveDownWords(2, 2, 3, words);

	for (int i = 1; i < words[0].size; i++){

		printf("%s\n", words[i].string);

	}


	return 0;
}
