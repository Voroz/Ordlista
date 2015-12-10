#ifndef _fileio_h
#define _fileio_h

#include "genlib.h"


static String currentExtension = "";


void setExtension(String extension);

String findExtension(String filename);

void appendFileExtension(String *filename);

FILE *openFile(String *filename, String accessMode);

void closeFile(FILE *file);

#endif