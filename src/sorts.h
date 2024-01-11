
// @NOTE: SortComparators return 1 if a > b, -1 if b < a, and 0 if they're equal
typedef int32 (*SortComparator)(void const *, void const *);
typedef int32 (*SortComparatorData)(void const *, void const *, void const *data);

int32 QuicksortPartition_(uint32 *array, size_t const elemSize, int32 const low, int32 const high) {
    uint32 pivot = array[low];

    int32 i = low;
    int32 j = high;

    while (true) {
        while (array[i] < pivot) {
            i++;
        }

        while (array[j] > pivot) {
            j--;            
        }

        if (i >= j) {
            return j;
        }

        if (i != j) {
            void *temp = alloca(elemSize);
            memcpy(temp, (uint8 *)array + (elemSize * i), elemSize);
            memcpy((uint8 *)array + (elemSize * i),
                    (uint8 *)array + (elemSize * j), elemSize);
            memcpy((uint8 *)array + (elemSize * j), temp, elemSize);
        }

        i++;
        j--;
    }
}

void Quicksort_(uint32 *array, size_t const elemSize, int32 const low, int32 const high) {
    if (low >= high) {
        return;
    }
    int32 mid = QuicksortPartition_(array, elemSize, low, high);
    Quicksort_(array, elemSize, low, mid);
    Quicksort_(array, elemSize, mid + 1, high);
}

void Quicksort(uint32 *array, size_t const elemSize, int32 const count) {
    Quicksort_(array, elemSize, 0, count - 1);
}


int32 QuicksortPartition_(void *array, size_t const elemSize, const int32 low, int32 const high, SortComparator comparator) {
    void *pivot = alloca(elemSize);
    memcpy(pivot, (uint8 *)array + (elemSize * low), elemSize);

    int32 i = low;
    int32 j = high;

    while (true) {
        while (comparator((uint8 *)array + (elemSize * i),  pivot) < 0) {
            i++;
        }

        while (comparator((uint8 *)array + (elemSize * j), pivot) > 0) {
            j--;            
        }

        if (i >= j) {
            return j;
        }

        if (i != j) {
            void *temp = alloca(elemSize);
            memcpy(temp, (uint8 *)array + (elemSize * i), elemSize);
            memcpy((uint8 *)array + (elemSize * i),
                    (uint8 *)array + (elemSize * j),
                    elemSize);
            memcpy((uint8 *)array + (elemSize * j),
                    temp,
                    elemSize);
        }

        i++;
        j--;
    }
}

void Quicksort_(void *array, size_t const elemSize, int32 const low, int32 const high, SortComparator comparator) {
    if (low >= high) {
        return;
    }
    int32 mid = QuicksortPartition_(array, elemSize, low, high, comparator);
    Quicksort_(array, elemSize, low, mid, comparator);
    Quicksort_(array, elemSize, mid + 1, high, comparator);
}

void Quicksort(void *array, size_t const elemSize, int32 const count, SortComparator comparator) {
    Quicksort_(array, elemSize, 0, count - 1, comparator);
}

int32 QuicksortPartition_(void *array, size_t const elemSize, const int32 low, int32 const high, SortComparator comparator, MemoryArena *arena) {
    void *pivot = PushSize(arena, elemSize);
    memcpy(pivot, (uint8 *)array + (elemSize * high), elemSize);

    int32 i = low - 1;

    for (int j = low; j <= high - 1; j++) {
        if (comparator((uint8 *)array + (elemSize * j),  pivot) <= 0) {
            i++;

            if (i != j) {
                void *temp = PushSize(arena, elemSize);
                memcpy(temp, (uint8 *)array + (elemSize * i), elemSize);
                memcpy((uint8 *)array + (elemSize * i),
                       (uint8 *)array + (elemSize * j),
                       elemSize);
                memcpy((uint8 *)array + (elemSize * j),
                       temp,
                       elemSize);
            }
        }
    }

    void *temp = PushSize(arena, elemSize);
    memcpy(temp, (uint8 *)array + (elemSize * (i + 1)), elemSize);
    memcpy((uint8 *)array + (elemSize * (i + 1)),
           (uint8 *)array + (elemSize * high),
           elemSize);
    memcpy((uint8 *)array + (elemSize * high),
           temp,
           elemSize);

    return i + 1;
}

// iterative version
void QuicksortIterative(void *array, size_t const elemSize, int32 const count, SortComparator comparator, MemoryArena *arena) {
    int32 low = 0;
    int32 high = count - 1;

    //DynamicArray<int32> stack = MakeDynamicArray<int32>(Game.frameMem, high - low + 1);
    int32 *stack = PushArray(arena, int32, high - low + 1);

    int32 top = -1;

    stack[++top] = low;
    stack[++top] = high;

    int32 startSize = arena->size;
    
    int32 pivot = 0;
    while (top >= 0) {

        high = stack[top--];
        low = stack[top--];

        arena->size = startSize;
        
        pivot = QuicksortPartition_(array, elemSize, low, high, comparator, arena);

        if (pivot - 1 > low) {
            stack[++top] = low;
            stack[++top] = pivot - 1;
        }

        if (pivot + 1 < high) {
            stack[++top] = pivot + 1;
            stack[++top] = high;
        } 
    }
    
}
