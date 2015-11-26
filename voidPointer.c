
typedef struct{
	int size;		          // Slots used so far
	int capacity;	         // Total available slots
	void **data;	        // Array of integers we're storing
}Vector;

int main()
{

Vector container;
char* string = "test";
Vector *words = malloc(100);
container.data = malloc(100);
words[0].data = malloc(100);


words[0].data = string;
container.data = words;
//printf(container.data[0].data);


return;
}
