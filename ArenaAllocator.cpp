#include <iostream>

struct Arena
{
    uint32_t size;
    uint32_t capacity;
    uint8_t* data;
    Arena* next = nullptr;
};

Arena arenaInit(size_t capacity)
{
    Arena arena;

    arena.size = 0;
    arena.capacity = capacity;
    arena.data = (uint8_t*)malloc(capacity);
    arena.next = nullptr;

    return arena;
}
void* arenaAllocate(Arena* arena, size_t objSize)
{
    Arena* current = arena;

    while(objSize > current->capacity - current->size )
    {
	  if (current->next == nullptr)
	  {
		Arena nextArena = arenaInit(current->capacity);
		Arena* nextArenaPtr = new Arena;
		memcpy(nextArenaPtr, &nextArena, sizeof(Arena));
		current->next = nextArenaPtr;
	  }
	  current = current->next;
    }
    void* data = &current->data[current->size];
    current->size += objSize;

    return data;
}

void arenaFree(Arena* arena)
{
    Arena* next;
    arena->capacity = 0;
    arena->size = 0;
    free(arena->data);
    Arena* current = arena->next;
    while (current->next != nullptr)
    {
	  next = current->next;
	  free(current->data);
	  free(current);
	  current = next;
    }
}



struct testObject
{
    char charArr[10];
    float i;
    int a;
    int b;
};

int main()
{
    {
	  Arena arena = arenaInit(512);
	  testObject* tests[35]{};
	  for (size_t i = 0; i < 35; i++)
	  {
		auto testObj = (testObject*)arenaAllocate(&arena, sizeof(testObject));
		tests[i] = testObj;
	  }
	  arenaFree(&arena);
    }

    {
	  Arena arena = arenaInit(512);
	  testObject* tests[35]{};
	  for (size_t i = 0; i < 35; i++)
	  {
		auto testObj = (testObject*)arenaAllocate(&arena, sizeof(testObject));
		tests[i] = testObj;
	  }
	  arenaFree(&arena);
    }
}
