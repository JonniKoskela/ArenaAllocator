#include <iostream>
#include <memory>
#include <vector>

class ArenaAllocator
{
public:
    explicit ArenaAllocator(size_t capacity)
    {
        head_ = std::make_unique<Arena>(capacity);
    }

    ~ArenaAllocator()
    {
    }

    template <typename T>
    [[nodiscard]] T* allocate()
    {
        return reinterpret_cast<T*>(allocateInternal(sizeof(T), alignof(T)));
    }

private:
    struct Arena
    {
        Arena(size_t capacity)
            : size(0), capacity(capacity), data(new uint8_t[capacity]), next(nullptr)
        {
        }

        ~Arena()
        {
            // No need to manually free data, unique_ptr will handle it
        }

        size_t size;
        size_t capacity;
        std::unique_ptr<uint8_t[]> data;
        std::unique_ptr<Arena> next;
    };

    void* allocateInternal(size_t objSize, size_t objAlignment)
    {
        Arena* current = head_.get();

        size_t alignedSize = (objSize + objAlignment - 1) & ~(objAlignment - 1);

        while (alignedSize > current->capacity - current->size)
        {
            if (!current->next)
            {
                current->next = std::make_unique<Arena>(current->capacity);
            }
            current = current->next.get();
        }

        void* data = &current->data[current->size];
        current->size += alignedSize;

        return data;
    }

    std::unique_ptr<Arena> head_;
};

struct TestObject
{
    char charArr[10];
    float i;
    int a;
    int b;
};

int main()
{
    ArenaAllocator arena(512);

    std::vector<TestObject*> tests;
    for (size_t i = 0; i < 35; ++i)
    {
        tests.push_back(arena.allocate<TestObject>());
    }

    return 0;
}
