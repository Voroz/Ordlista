#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

const int MAXWORDLENGTH = 100;

typedef struct{
	char* string;
	int size;
}Word;

Word* storeFileWords(char* filename){
	//Read from file
	FILE *file;
	if (!(file = fopen(filename, "r"))){
		printf("Error opening %s\n", filename);
		return;
	}

	//Read size and allocate memory
	Word *words;
	int arraySize = 0;
	fscanf(file, "%d //Amount of words\n", &arraySize);
	words = malloc(arraySize*sizeof(Word));

	//First word contains size of array
	words[0].size = arraySize;

	//Allocate memory for string length and save in array
	words[1].string = malloc(MAXWORDLENGTH * sizeof(char));  //Temp memory
	for (int i = 1; fscanf(file, "%s\n", words[i].string) != EOF; i++){

        //Reallocate memory to match word size
		words[i].size = strlen(words[i].string);
		words[i].string = realloc(words[i].string, words[i].size * sizeof(char*));   // Behövs?, Varför char*?

		//Allocate room for next word (unless we are at last word)
		if (i < arraySize - 1){
			words[i + 1].string = malloc(MAXWORDLENGTH * sizeof(char)); //Temp memory
		}
	}

	fclose(file);
	return words;
}

/*Will find the proper placement for the word we give the function based on the alphabet
  Will be used inside the addWord function(Not yet done!!).*/
void findPlaceForWord(char* word){
    int position;
    return position;
}

//Using this in add word function
void moveDownWords(int firstWordPos, int amountOfWords, int distance, Word **words){
    int lastWordPos = firstWordPos + amountOfWords-1,
        newSize = lastWordPos + distance+1,
		originalSize = words[0]->size,
		sizeDifference = newSize - words[0]->size;

    //Exit function if input doesn't make sense
    if (lastWordPos >= originalSize || firstWordPos <= 0 || amountOfWords <= 0){
        printf("Error! You are trying to move memory that you don't own!");
        return;
    }

    //Allocate just enough memory to be able to move down words to the right place.
	if (sizeDifference > 0){
		words[0]->size = newSize;

		//Reallocate Words memory
		*words = realloc(*words, (words[0]->size)*sizeof(Word));

        //Allocate memory for the strings inside our new Words
        //(The empty space that might happen after moving words down far enough).
		for (int i = originalSize; i < words[0]->size; i++){
			(*words)[i].string = malloc(MAXWORDLENGTH*sizeof(char));    //Här kan vi inte använda ->. Varför? (Order of precedence)
		}
	}
	memmove(*words + firstWordPos + distance, *words + firstWordPos, amountOfWords*sizeof(Word));
	return;
}

//Using this in delete word function
void moveUpWords(int firstWordPos, int amountOfWords, int distance, Word **words){
    int lastWordPos = firstWordPos + amountOfWords-1,
        newSize = lastWordPos - distance+1,
		originalSize = words[0]->size,
		sizeDifferance = words[0]->size - newSize;

    //Exit function if input doesn't make sense
    if (lastWordPos >= originalSize || firstWordPos <= 0){
        printf("Error! You are trying to move memory that you don't own!");
        return;
    }

    memmove(*words + firstWordPos - distance, *words + firstWordPos, amountOfWords*sizeof(Word));

    //Free memory only if we are moving the last word in the array
    if (lastWordPos == words[0]->size - 1){
        words[0]->size = newSize;

        //Reallocate Words memory
		*words = realloc(*words, (words[0]->size)*sizeof(Word));
    }
    return;
}

void addWord(char* word, int position){

}

//Find word and return word position
int findWord(char* word, Word *words){
	for (int i = 1; i < words[0].size; i++){
		//Check if strings match with memcmp
		if (memcmp(word, words[i].string, words[i].size) == 0){
			return i; // i = position
		}
	}
	return NULL;
}

void deleteWord(int position, Word **words){
    int distance = 1,
        firstWordPos = position+1,
        amountOfWords = words[0]->size - firstWordPos,
        lastWordPos = firstWordPos + amountOfWords-1,
        newSize = lastWordPos - distance+1;

    if (position == NULL){
        printf("Error! Word doesn't exist!\n");
        return;
    }

    //If the word is the last word, realloc but don't try to move memory.
    if (amountOfWords <= 0){
        words[0]->size = newSize;

        //Reallocate Words memory
		*words = realloc(*words, (words[0]->size)*sizeof(Word));
		return;
    }

    //Move up words below the word to be deleted
    moveUpWords(firstWordPos, amountOfWords, distance, &(*words));
    return;
}


int main(){

Word *words;

words = storeFileWords("ordlista.txt");

deleteWord(findWord("Akrobat", words), &words);
deleteWord(findWord("Abstinens", words), &words);
deleteWord(findWord("Betalmedel", words), &words);
deleteWord(findWord("Betalmedel", words), &words);

for (int i = 1; i < words[0].size; i++){
    printf("%s\n", words[i].string);
}


return 0;
}
