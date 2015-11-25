#pragma warning(disable:4996)
/*
 * File: simpio.c
 * Version: 1.0
 * Last modified on Fri Jul 15 14:10:41 1994 by eroberts
 * -----------------------------------------------------
 * This file implements the simpio.h interface.
 */

#include <stdio.h>
#include <String.h>

#include "genlib.h"
#include "strlib.h"
#include "simpio.h"

/*
 * Constants:
 * ----------
 * InitialBufferSize -- Initial buffer size for ReadLine
 */

#define InitialBufferSize 120

/* Exported entries */

/*
 * Functions: GetInteger, GetLong, GetReal
 * ---------------------------------------
 * These functions first read a line and then call sscanf to
 * translate the number.  Reading an entire line is essential to
 * good error recovery, because the characters after the point of
 * error would otherwise remain in the input buffer and confuse
 * subsequent input operations.  The sscanf line allows white space
 * before and after the number but no other extraneous characters.
 */

int GetInteger(void)
{
    String line;
    int value;
    char termch;

    while (TRUE) {
        line = GetLine();
        switch (sscanf(line, " %d %c", &value, &termch)) {
          case 1:
            FreeBlock(line);
            return (value);
          case 2:
            printf("Unexpected character: '%c'\n", termch);
            break;
          default:
            printf("Please enter an integer\n");
            break;
        }
        FreeBlock(line);
        printf("Retry: ");
    }
}

long GetLong(void)
{
    String line;
    long value;
    char termch;

    while (TRUE) {
        line = GetLine();
        switch (sscanf(line, " %ld %c", &value, &termch)) {
          case 1:
            FreeBlock(line);
            return (value);
          case 2:
            printf("Unexpected character: '%c'\n", termch);
            break;
          default:
            printf("Please enter an integer\n");
            break;
        }
        FreeBlock(line);
        printf("Retry: ");
    }
}

double GetReal(void)
{
    String line;
    double value;
    char termch;

    while (TRUE) {
        line = GetLine();
        switch (sscanf(line, " %lf %c", &value, &termch)) {
          case 1:
            FreeBlock(line);
            return (value);
          case 2:
            printf("Unexpected character: '%c'\n", termch);
            break;
          default:
            printf("Please enter a real number\n");
            break;
        }
        FreeBlock(line);
        printf("Retry: ");
    }
}

/*
 * Function: GetLine
 * -----------------
 * This function is a simple wrapper; all the work is done by
 * ReadLine.
 */

String GetLine(void)
{
    return (ReadLine(stdin));
}

/*
 * Function: ReadLine
 * ------------------
 * This function operates by reading characters from the file
 * into a dynamically allocated buffer.  If the buffer becomes
 * full before the end of the line is reached, a new buffer
 * twice the size of the previous one is allocated.
 */

String ReadLine(FILE *infile)
{
    String line, nline;
    int n, ch, size;

    n = 0;
    size = InitialBufferSize;
    line = GetBlock(size + 1);
    while ((ch = getc(infile)) != '\n' && ch != EOF) {
        if (n == size) {
            size *= 2;
            nline = (String) GetBlock(size + 1);
            strncpy(nline, line, n);
            FreeBlock(line);
            line = nline;
        }
        line[n++] = ch;
    }
    if (n == 0 && ch == EOF) {
        FreeBlock(line);
        return (NULL);
    }
    line[n] = '\0';
    nline = (String) GetBlock(n + 1);
    strcpy(nline, line);
    FreeBlock(line);
    return (nline);
}
