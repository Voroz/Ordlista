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
    char *currentString = (char*)malloc(40*sizeof(char));
    for (int i = 1; fscanf(file, "%s\n", currentString) != EOF; i++){
        words[i].size = strlen(currentString);
        words[i].string = (char*)malloc(words[i].size * sizeof(char*));
        words[i].string = currentString;
    }
    fclose(file);
		return;
}

int main(){

Words *words;

storeFileWords("ordlista.txt", words);


return 0;
}
