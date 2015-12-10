#ifndef _vector_h
#define _vector_h

typedef struct{
	int size;		// Slots used so far
	int capacity;	// Total available slots
	void **data;	// Array of data we're storing
} Vector;

void vectorInit(Vector *pVector);

int vectorSize(Vector *pVector);

static void** vectorCopyValue(void* *value, int sizeOfElem);

static void vectorFreeValue(void* value);

static void vectorDoubleCapacityIfFull(Vector *pVector);

static void vectorHalfCapacityIfNotUsed(Vector *pVector);

void vectorAppend(Vector *pVector, void* *value, int sizeOfElem);

void vectorSet(Vector *pVector, int index, void* *value);

void* vectorGet(Vector *pVector, int index);

// Moves all values one step higer from index position and inserts the new value at index
void vectorInsert(Vector *pVector, int index, void* *value, int sizeOfElem);

void vectorRemove(Vector *pVector, int index);

void vectorFree(Vector *pVector);

void vectorClear(Vector *pVector);

#endif
