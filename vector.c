#include <stdlib.h>

#include "vector.h"

#define VECTOR_INITIAL_CAPACITY 10

void vectorInit(Vector *pVector){
	// Initialize size and capacity
	pVector->size = 0;
	pVector->capacity = VECTOR_INITIAL_CAPACITY;

	// Allocate memory for vector->data
	pVector->data = GetBlock(sizeof(void*)* pVector->capacity);
}

int vectorSize(Vector *pVector){
	return pVector->size;
}

static void** vectorCopyValue(void* *value, int sizeOfElem){
	void* ptr = GetBlock(sizeOfElem);
	memcpy(ptr, value, sizeOfElem);
	return ptr;
}

static void vectorFreeValue(void* value){
	free(value);
}

static void vectorDoubleCapacityIfFull(Vector *pVector){
	if (pVector->size >= pVector->capacity){
		void **newMemory = realloc(pVector->data, sizeof(void*)* (pVector->capacity * 2));
		if (newMemory){
			#ifdef DEBUG_ON
				printf("Vector resize: %d to %d\n", pVector->size, (pVector->capacity * 2));
			#endif
			pVector->data = newMemory;
			pVector->capacity *= 2;
		}
	}
}

static void vectorHalfCapacityIfNotUsed(Vector *pVector){
	if (pVector->capacity / pVector->size >= 2){
		void **newMemory = realloc(pVector->data, sizeof(void*)* (pVector->capacity / 2));
		if (newMemory){
			#ifdef DEBUG_ON
				printf("Vector resize: %d to %d\n", pVector->capacity, pVector->capacity / 2);
			#endif
			pVector->data = newMemory;
			pVector->capacity /= 2;
		}
	}
}

void vectorAppend(Vector *pVector, void* *value, int sizeOfElem){
	// Make sure there's room to expand into
	vectorDoubleCapacityIfFull(pVector);

	// Append the value and increment vector->size
	pVector->data[pVector->size++] = vectorCopyValue(value, sizeOfElem);
}

void vectorSet(Vector *pVector, int index, void* *value){
	if (index >= pVector->size || index < 0){
		#ifdef DEBUG_ON
			printf("'vectorSet' - Index %d is out of bounds for vector of size %d\n", index, pVector->size);
		#endif
	}
	else{
		// Set the value at the desired index
		pVector->data[index] = value;
	}
}

void* vectorGet(Vector *pVector, int index){
	if (index >= pVector->size || index < 0){
		#ifdef DEBUG_ON
			printf("'vectorGet' - Index %d is out of bounds for vector of size %d\n", index, pVector->size);
		#endif
		return NULL; //##########// TODO: Testa!
	}
	else{
		return pVector->data[index];
	}
}

//
// Moves all values one step higer from index position and inserts the new value at index
//
void vectorInsert(Vector *pVector, int index, void* *value, int sizeOfElem){
	// Check if out of bounds
	if (index < 0 || index >= pVector->size){
		#ifdef DEBUG_ON
			printf("'vectorInsert' - Index %d is out of bounds for vector of size %d\n", index, pVector->size);
		#endif
	}
	else{
		// Make the vector one element larger to make room for the new value
		pVector->size++;
		vectorDoubleCapacityIfFull(pVector);

		// Move all values whos index is larger than 'index' one higher (element 5 becomes 4, 4 becomes 3, and so on)
		for (int i = (pVector->size - 1); i > index; i--){
			vectorSet(pVector, i, pVector->data[i - 1]);
		}
		// Save the value at a new adress
		void* ptr = vectorCopyValue(value, sizeOfElem);

		// Save the new adress in the vector
		vectorSet(pVector, index, ptr);
	}
}

void vectorRemove(Vector *pVector, int index){
	// Check if out of bounds
	if (index < 0 || index >= pVector->size){
		#ifdef DEBUG_ON
			printf("'vectorRemove' - Index %d is out of bounds for vector of size %d\n", index, pVector->size);
		#endif
	}
	else{
		// Check usage and halves vector if usage is <= 50%
		vectorHalfCapacityIfNotUsed(pVector);

		// Remove value att index position
		vectorFreeValue(pVector->data[index]);
		// Move all values whos index is larger than 'index' one lower (element 4 becomes 5, 5 becomes 6, and so on)
		for (int i = index; i < (pVector->size - 1); i++){
			vectorSet(pVector, i, pVector->data[i + 1]);
		}
		// Decrement vector->size and set last value to NULL
		vectorSet(pVector, (pVector->size - 1), NULL);
		pVector->size--;
	}
}

void vectorFree(Vector *pVector){
	for (int i = 0; i < pVector->size; i++){
		vectorFreeValue(pVector->data[i]);
	}
	free(pVector->data);
}

void vectorClear(Vector *pVector){
	vectorFree(pVector);
	vectorInit(pVector);
}
