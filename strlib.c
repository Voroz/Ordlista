#pragma warning(disable:4996)
/*
 * File: strlib.c
 * Version: 1.0
 * Last modified on Fri Jul 15 14:10:41 1994 by eroberts
 * -----------------------------------------------------
 * This file implements the strlib.h interface.
 */

/*
 * General implementation notes:
 * -----------------------------
 * This module implements the strlib library by mapping all
 * functions into the appropriate calls to the ANSI <String.h>
 * interface.  The implementations of the individual functions
 * are all quite simple and do not require individual comments.
 * For descriptions of the behavior of each function, see the
 * interface.
 */

#include <stdio.h>
#include <String.h>
#include <ctype.h>

#include "genlib.h"
#include "strlib.h"

/*
 * Constant: MaxDigits
 * -------------------
 * This constant must be larger than the maximum
 * number of digits that can appear in a number.
 */

#define MaxDigits 30

/* Private function prototypes */

static String CreateString(int len);

/* Section 1 -- Basic String operations */

String Concat(String s1, String s2)
{
    String s;
    int len1, len2;

    if (s1 == NULL || s2 == NULL) {
        Error("NULL String passed to Concat");
    }
    len1 = strlen(s1);
    len2 = strlen(s2);
    s = CreateString(len1 + len2);
    strcpy(s, s1);
    strcpy(s + len1, s2);
    return (s);
}

char IthChar(String s, int i)
{
    int len;

    if (s == NULL) Error("NULL String passed to IthChar");
    len = strlen(s);
    if (i < 0 || i > len) {
        Error("Index outside of String range in IthChar");
    }
    return (s[i]);
}

String SubString(String s, int p1, int p2)
{
    int len;
    String result;

    if (s == NULL) Error("NULL String passed to SubString");
    len = strlen(s);
    if (p1 < 0) p1 = 0;
    if (p2 >= len) p2 = len - 1;
    len = p2 - p1 + 1;
    if (len < 0) len = 0;
    result = CreateString(len);
    strncpy(result, s + p1, len);
    result[len] = '\0';
    return (result);
}

String CharToString(char ch)
{
    String result;

    result = CreateString(1);
    result[0] = ch;
    result[1] = '\0';
    return (result);
}

int StringLength(String s)
{
    if (s == NULL) Error("NULL String passed to StringLength");
    return (strlen(s));
}

String CopyString(String s)
{
    String newstr;

    if (s == NULL) Error("NULL String passed to CopyString");
    newstr = CreateString(strlen(s));
    strcpy(newstr, s);
    return (newstr);
}

/* Section 2 -- String comparison functions */

bool StringEqual(String s1, String s2)
{
    if (s1 == NULL || s2 == NULL) {
        Error("NULL String passed to StringEqual");
    }
    return (strcmp(s1, s2) == 0);
}

int StringCompare(String s1, String s2)
{
    if (s1 == NULL || s2 == NULL) {
        Error("NULL String passed to StringCompare");
    }
    return (strcmp(s1, s2));
}

/* Section 3 -- Search functions */

int FindChar(char ch, String text, int start)
{
    char *cptr;

    if (text == NULL) Error("NULL String passed to FindChar");
    if (start < 0) start = 0;
    if (start > strlen(text)) return (-1);
    cptr = strchr(text + start, ch);
    if (cptr == NULL) return (-1);
    return ((int) (cptr - text));
}

//New function
int FindCharFromRight(char ch, String text, int start)
{
    char *cptr;

    if (text == NULL) Error("NULL String passed to FindChar");
    if (start < 0) start = 0;
    if (start > strlen(text)) return (-1);
    cptr = strrchr(text + start, ch);
    if (cptr == NULL) return (-1);
    return ((int) (cptr - text));
}

int FindString(String str, String text, int start)
{
    char *cptr;

    if (str == NULL) Error("NULL pattern String in FindString");
    if (text == NULL) Error("NULL text String in FindString");
    if (start < 0) start = 0;
    if (start > strlen(text)) return (-1);
    cptr = strstr(text + start, str);
    if (cptr == NULL) return (-1);
    return ((int) (cptr - text));
}

/* Section 4 -- Case-conversion functions */

String ConvertToLowerCase(String s)
{
    String result;
    int i;

    if (s == NULL) {
        Error("NULL String passed to ConvertToLowerCase");
    }
    result = CreateString(strlen(s));
    for (i = 0; s[i] != '\0'; i++) result[i] = tolower(s[i]);
    result[i] = '\0';
    return (result);
}

String ConvertToUpperCase(String s)
{
    String result;
    int i;

    if (s == NULL) {
        Error("NULL String passed to ConvertToUpperCase");
    }
    result = CreateString(strlen(s));
    for (i = 0; s[i] != '\0'; i++) result[i] = toupper(s[i]);
    result[i] = '\0';
    return (result);
}

/* Section 5 -- Functions for converting numbers to strings */

String IntegerToString(int n)
{
    char buffer[MaxDigits];

    sprintf(buffer, "%d", n);
    return (CopyString(buffer));
}

int StringToInteger(String s)
{
    int result;
    char dummy;

    if (s == NULL) {
        Error("NULL String passed to StringToInteger");
    }
    if (sscanf(s, " %d %c", &result, &dummy) != 1) {
        Error("StringToInteger called on illegal number %s", s);
    }
    return (result);
}

String RealToString(double d)
{
    char buffer[MaxDigits];

    sprintf(buffer, "%G", d);
    return (CopyString(buffer));
}

double StringToReal(String s)
{
    double result;
    char dummy;

    if (s == NULL) Error("NULL String passed to StringToReal");
    if (sscanf(s, " %lg %c", &result, &dummy) != 1) {
        Error("StringToReal called on illegal number %s", s);
    }
    return (result);
}

/* Private functions */

/*
 * Function: CreateString
 * Usage: s = CreateString(len);
 * -----------------------------
 * This function dynamically allocates space for a String of
 * len characters, leaving room for the null character at the
 * end.
 */

static String CreateString(int len)
{
    return ((String) GetBlock(len + 1));
}
