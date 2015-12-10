#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <stdio.h>
#include "genlib.h"
#include "errorhand.h"
#include "fileio.h"
#include "strmanip.h"
#include "strlib.h"
#include "strmanip.h"

void setExtension(String extension){
	currentExtension = extension;
}

String findExtension(String filename){
	return SubString(filename, FindCharFromRight('.', filename, 0), StringLength(filename));
}

void appendFileExtension(String *filename){
	// Return if there's already an extension
	String ext = findExtension(*filename);
	if (*ext == '.'){
		#ifdef DEBUG_ON
			printf("'appendFileExtension' - File already has an extension.\n");
		#endif
	}
	else{
		String temp = Concat(*filename, ".");
		*filename = Concat(temp, currentExtension);
		FreeBlock(temp);
	}
	FreeBlock(ext);
}

FILE *openFile(String *filename, String accessMode){
	FILE *file;
	if (stringIsEmpty(*filename) /*|| *filename[0] == '.'*/){
		userError(noFilename);
	}
	String fileCpy = CopyString(*filename);
	// Add extension to 'fileCpy' if not already there
	appendFileExtension(&fileCpy);

	// Open file and check for errors
	if (!(file = fopen(fileCpy, accessMode))){
		FreeBlock(fileCpy);
		userError(fileOpen, *filename);
	}
	FreeBlock(fileCpy);
	return file;
}

void closeFile(FILE *file){
	if (fclose(file) == EOF){
		Error("Error while closing file");
	}
}