#ifndef _fileio_h
#define _fileio_h

#include <stdio.h>

#include "genlib.h"


/*
* Static: currentExtension
* -------------------
* Used to store the current extension to add in openFile function.
*/
static String currentExtension = "";

/*
* Function: setExtension
* Usage: setExtension(extension);
* -------------------------------------
* Set the current extension to append to filenames.
*/
void setExtension(String extension);

/*
* Function: findExtension
* Usage: String extension = findExtension(filename);
* -------------------------------------
* Returns a new string containing the extension of string 'filename'.
*/
String findExtension(String filename);

/*
* Function: appendFileExtension
* Usage: appendFileExtension(filename);
* -------------------------------------
* Adds the 'currentExtension' to string 'filename'.
* If filename already have extension no extra filename will be appended.
*/
void appendFileExtension(String *filename);

/*
* Function: openFile
* Usage: FILE *file = openFile(filename, accessMode);
* -------------------------------------
* Returns a FILE* object if succesfull or gives an error message if something went wrong.
* 'accessMode' is one the mode settings the standard c function fopen() accepts.
* This function will check for extension in 'filename' and if no extesnion is found will try to add 'currentExtension'
*/
FILE* openFile(String *filename, String accessMode);

/*
* Function: closeFile
* Usage: closeFile(file);
* -------------------------------------
* Closes the filestream 'file'.
* If it fails an error is thrown.
*/
void closeFile(FILE *file);

#endif