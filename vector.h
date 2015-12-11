#ifndef _vector_h
#define _vector_h

/*
* Struct: Vector
* -------------------
* The struct that keeps track of size and usage of vector.
* 'data' points to the array of data we are storing.
*/
typedef struct vector{
	int size;		// Slots used so far
	int capacity;	// Total available slots
	void **data;	// Array of data we're storing
} Vector;

/*
* Function: vectorInit
* Usage: vectorInit(pVector);
* -------------------------------------
* initializes a new vector with a size for 10 elements.
*/
void vectorInit(Vector *pVector);

/*
* Function: vectorSize
* Usage: int n = vectorSize(pVector);
* -------------------------------------
* Returns the amount of elements in vector.
*/
int vectorSize(Vector *pVector);

/*
* Function: vectorAppend
* Usage: vectorAppend(pVector, value, sizeOfElem);
* -------------------------------------
* Append a new value to last position in vector.
*/
void vectorAppend(Vector *pVector, void* *value, int sizeOfElem);

/*
* Function: vectorSet
* Usage: vectorSet(pVector, index, value);
* -------------------------------------
* Position 'index' in vector gets the new value passed in.
*/
void vectorSet(Vector *pVector, int index, void* *value);

/*
* Function: vectorGet
* Usage: type *a = vectorGet(pVector, index);
* -------------------------------------
* Returns a pointer to the element at position 'index'.
*/
void* vectorGet(Vector *pVector, int index);

/*
* Function: vectorInsert
* Usage: vectorInsert(pVector, index, value, sizeOfElem);
* -------------------------------------
* Inserts 'value' at position 'index'.
*/
void vectorInsert(Vector *pVector, int index, void* *value, int sizeOfElem);

/*
* Function: vectorRemove
* Usage: vectorRemove(pVector, index);
* -------------------------------------
* Removes the element at position 'index'.
*/
void vectorRemove(Vector *pVector, int index);

/*
* Function: vectorFree
* Usage: vectorFree(pVector);
* -------------------------------------
* Frees all the memory the vector has allocated.
*/
void vectorFree(Vector *pVector);

/*
* Function: vectorClear
* Usage: vectorClear(pVector);
* -------------------------------------
* Removes all elements from vector and returns size to original size 10.
*/
void vectorClear(Vector *pVector);

#endif
