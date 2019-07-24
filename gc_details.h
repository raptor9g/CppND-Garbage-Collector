// This class defines an element that is stored
// in the garbage collection information list.
//
template <class T>
class PtrDetails
{
  public:
    unsigned refcount = 1; // current reference count
    T *memPtr = nullptr;         // pointer to allocated memory
    /* isArray is true if memPtr points
to an allocated array. It is false
otherwise. */
    bool isArray; // true if pointing to array
    /* If memPtr is pointing to an allocated
array, then arraySize contains its size */
    unsigned arraySize; // size of array
    // Here, mPtr points to the allocated memory.
    // If this is an array, then size specifies
    // the size of the array.

    PtrDetails(T *memPtr, unsigned arraySize):
      memPtr(memPtr), isArray(true), arraySize(arraySize)
    {
        // TODO: Implement PtrDetails
    }
    PtrDetails(T *memPtr):
      memPtr(memPtr), isArray(false)
    {
        // TODO: Implement PtrDetails
    }
};
// Overloading operator== allows two class objects to be compared.
// This is needed by the STL list class.
//I took a diffrent approach.
/* template <class T>
bool operator==(const PtrDetails<T> &ob1,
                const PtrDetails<T> &ob2)
{
    // TODO: Implement operator==
} */