#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

typedef struct{
    char* string;
    size_t size;
}Words;

void storeFileWords(char* filename, Words *words){
    //Read from file
    FILE *file;
    if (!(file = fopen(filename, "r"))){
        printf("Error opening %s\n", filename);
        return 0;
    }

    //Read size and allocate memory
    int arraySize = 0;
    fscanf(file, "%d //Amount of words\n", &arraySize);
    words = (Words*)malloc(arraySize*sizeof(Words));
    //First word contains size of array
    words[0].size = arraySize;

    //Allocate memory for string length and save in array
    char *currentString = (char*)malloc(100*sizeof(char)); //Temp memory
    for (int i = 1; fscanf(file, "%s\n", currentString) != EOF; i++){
        words[i].size = strlen(currentString);
        words[i].string = (char*)realloc(currentString, words[i].size * sizeof(char*));
        words[i].string = currentString;
    }
    fclose(file);
    return;
}

void addWord(char* word, int position){

}

//Find word and return word position
int findWord(char* word){
    int position;
    /*
    ...Code
    */
    return position;
}

//Move word down 1 position
void moveDownWords(int firstWordPos, int amountOfWords, int newPos){
    /*
    ...memcpy the words?
    */
}

int main(){

Words *words;

storeFileWords("ordlista.txt", words);


return 0;
}
