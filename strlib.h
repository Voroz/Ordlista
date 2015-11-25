/*
 * File: strlib.h
 * Version: 1.0
 * Last modified on Fri Jul 15 14:10:40 1994 by eroberts
 * -----------------------------------------------------
 * The strlib.h file defines the interface for a simple
 * String library.  In the context of this package, strings
 * are considered to be an abstract data type, which means
 * that the client relies only on the operations defined for
 * the type and not on the underlying representation.
 */

/*
 * Cautionary note:
 * ----------------
 * Although this interface provides an extremely convenient
 * abstraction for working with strings, it is not appropriate
 * for all applications.  In this interface, the functions that
 * return String values (such as Concat and SubString) do so
 * by allocating new memory.  Over time, a program that uses
 * this package will consume increasing amounts of memory
 * and eventually exhaust the available supply.  If you are
 * writing a program that runs for a short time and stops,
 * the fact that the package consumes memory is not a problem.
 * If, however, you are writing an application that must run
 * for an extended period of time, using this package requires
 * that you make some provision for freeing any allocated
 * storage.
 */

#ifndef _strlib_h
#define _strlib_h

#include "genlib.h"

/* Section 1 -- Basic String operations */

/*
 * Function: Concat
 * Usage: s = Concat(s1, s2);
 * --------------------------
 * This function concatenates two strings by joining them end
 * to end.  For example, Concat("ABC", "DE") returns the String
 * "ABCDE".
 */

String Concat(String s1, String s2);

/*
 * Function: IthChar
 * Usage: ch = IthChar(s, i);
 * --------------------------
 * This function returns the character at position i in the
 * String s.  It is included in the library to make the type
 * String a true abstract type in the sense that all of the
 * necessary operations can be invoked using functions. Calling
 * IthChar(s, i) is like selecting s[i], except that IthChar
 * checks to see if i is within the range of legal index
 * positions, which extend from 0 to StringLength(s).
 * IthChar(s, StringLength(s)) returns the null character
 * at the end of the String.
 */

char IthChar(String s, int i);

/*
 * Function: SubString
 * Usage: t = SubString(s, p1, p2);
 * --------------------------------
 * SubString returns a copy of the substring of s consisting
 * of the characters between index positions p1 and p2,
 * inclusive.  The following special cases apply:
 *
 * 1. If p1 is less than 0, it is assumed to be 0.
 * 2. If p2 is greater than the index of the last String
 *    position, which is StringLength(s) - 1, then p2 is
 *    set equal to StringLength(s) - 1.
 * 3. If p2 < p1, SubString returns the empty String.
 */

String SubString(String s, int p1, int p2);

/*
 * Function: CharToString
 * Usage: s = CharToString(ch);
 * ----------------------------
 * This function takes a single character and returns a
 * one-character String consisting of that character.  The
 * CharToString function is useful, for example, if you
 * need to concatenate a String and a character.  Since
 * Concat requires two strings, you must first convert
 * the character into a String.
 */

String CharToString(char ch);

/*
 * Function: StringLength
 * Usage: len = StringLength(s);
 * -----------------------------
 * This function returns the length of s.
 */

int StringLength(String s);

/*
 * Function: CopyString
 * Usage: newstr = CopyString(s);
 * ------------------------------
 * CopyString copies the String s into dynamically allocated
 * storage and returns the new String.  This function is not
 * ordinarily required if this package is used on its own,
 * but is often necessary when you are working with more than
 * one String package.
 */

String CopyString(String s);

/* Section 2 -- String comparison functions */

/*
 * Function: StringEqual
 * Usage: if (StringEqual(s1, s2)) ...
 * -----------------------------------
 * This function returns TRUE if the strings s1 and s2 are
 * equal.  For the strings to be considered equal, every
 * character in one String must precisely match the
 * corresponding character in the other.  Uppercase and
 * lowercase characters are considered to be different.
 */

bool StringEqual(String s1, String s2);

/*
 * Function: StringCompare
 * Usage: if (StringCompare(s1, s2) < 0) ...
 * -----------------------------------------
 * This function returns a number less than 0 if String s1
 * comes before s2 in alphabetical order, 0 if they are equal,
 * and a number greater than 0 if s1 comes after s2.  The
 * ordering is determined by the internal representation used
 * for characters, which is usually ASCII.
 */

int StringCompare(String s1, String s2);

/* Section 3 -- Search functions */

/*
 * Function: FindChar
 * Usage: p = FindChar(ch, text, start);
 * -------------------------------------
 * Beginning at position start in the String text, this
 * function searches for the character ch and returns the
 * first index at which it appears or -1 if no match is
 * found.
 */

int FindChar(char ch, String text, int start);

/*
 * Function: FindString
 * Usage: p = FindString(str, text, start);
 * ----------------------------------------
 * Beginning at position start in the String text, this
 * function searches for the String str and returns the
 * first index at which it appears or -1 if no match is
 * found.
 */

int FindString(String str, String text, int start);

/* Section 4 -- Case-conversion functions */

/*
 * Function: ConvertToLowerCase
 * Usage: s = ConvertToLowerCase(s);
 * ---------------------------------
 * This function returns a new String with all
 * alphabetic characters converted to lower case.
 */

String ConvertToLowerCase(String s);

/*
 * Function: ConvertToUpperCase
 * Usage: s = ConvertToUpperCase(s);
 * ---------------------------------
 * This function returns a new String with all
 * alphabetic characters converted to upper case.
 */

String ConvertToUpperCase(String s);

/* Section 5 -- Functions for converting numbers to strings */

/*
 * Function: IntegerToString
 * Usage: s = IntegerToString(n);
 * ------------------------------
 * This function converts an integer into the corresponding
 * String of digits.  For example, IntegerToString(123)
 * returns "123" as a String.
 */

String IntegerToString(int n);

/*
 * Function: StringToInteger
 * Usage: n = StringToInteger(s);
 * ------------------------------
 * This function converts a String of digits into an integer.
 * If the String is not a legal integer or contains extraneous
 * characters, StringToInteger signals an error condition.
 */

int StringToInteger(String s);

/*
 * Function: RealToString
 * Usage: s = RealToString(d);
 * ---------------------------
 * This function converts a floating-point number into the
 * corresponding String form.  For example, calling
 * RealToString(23.45) returns "23.45".  The conversion is
 * the same as that used for "%G" format in printf.
 */

String RealToString(double d);

/*
 * Function: StringToReal
 * Usage: d = StringToReal(s);
 * ---------------------------
 * This function converts a String representing a real number
 * into its corresponding value.  If the String is not a
 * legal floating-point number or if it contains extraneous
 * characters, StringToReal signals an error condition.
 */

double StringToReal(String s);

#endif
