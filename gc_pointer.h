#include <iostream>
#include <map>
#include <mutex>
#include <utility>
#include <typeinfo>
#include <cstdlib>
#include "gc_details.h"
#include "gc_iterator.h"
/*
    Pointer implements a pointer type that uses
    garbage collection to release unused memory.
    A Pointer must only be used to point to memory
    that was dynamically allocated using new.
    When used to refer to an allocated array,
    specify the array size.
*/

template <class T, int size = 0>
class Pointer{
private:
    static std::mutex _mutex_;
    // refContainer maintains the garbage collection map.
    static std::map<T*, PtrDetails<T> > refContainer;
    // addr points to the allocated memory to which
    // this Pointer pointer currently points.
    T *addr = nullptr;
    /*  isArray is true if this Pointer points
        to an allocated array. It is false
        otherwise. 
    */
    bool isArray = size > 0; 
    // true if pointing to array
    // If this Pointer is pointing to an allocated
    // array, then arraySize contains its size.
    unsigned arraySize = size; // size of the array
    static bool first; // true when first Pointer is created

    // Return an iterator to pointer details in refContainer.
    // typename std::list<PtrDetails<T> >::iterator findPtrInfo(T *ptr);
public:
    // Define an iterator type for Pointer<T>.
    typedef Iter<T> GCiterator;
    // Empty constructor
    // NOTE: templates aren't able to have prototypes with default arguments
    // this is why constructor is designed like this:
   /*  Pointer(){
        Pointer(nullptr);
    } */
    Pointer(T*);
    // Copy constructor.
    Pointer(const Pointer &);
    // Destructor for Pointer.
    ~Pointer();
    // Collect garbage. Returns true if at least
    // one object was freed.
    
    // static bool collect(); no
    // Overload assignment of pointer to Pointer.
    T *operator=(T *t);
    // Overload assignment of Pointer to Pointer.
    Pointer &operator=(Pointer &rv);
    // Return a reference to the object pointed
    // to by this Pointer.
    T &operator*(){
        return *addr;
    }
    // Return the address being pointed to.
    T *operator->() { return addr; }
    // Return a reference to the object at the
    // index specified by i.
    T &operator[](int i){ return addr[i];}
    // Conversion function to T *.
    operator T *() { return addr; }
    // Return an Iter to the start of the allocated memory.
    Iter<T> begin(){
        int _size;
        if (isArray)
            _size = arraySize;
        else
            _size = 1;
        return Iter<T>(addr, addr, addr + _size);
    }
    // Return an Iter to one past the end of an allocated array.
    Iter<T> end(){
        int _size;
        if (isArray)
            _size = arraySize;
        else
            _size = 1;
        return Iter<T>(addr + _size, addr, addr + _size);
    }
    // Return the size of refContainer for this type of Pointer.
    static int refContainerSize() { return refContainer.size(); }
    // A utility function that displays refContainer.
    static void showlist();
    // Clear refContainer when program exits.
    static void shutdown();
};

// STATIC INITIALIZATION
// Creates storage for the static variables
template <class T, int size>
std::mutex Pointer<T, size>::_mutex_; 

template <class T, int size>
std::map<T*, PtrDetails<T> > Pointer<T, size>::refContainer;

template <class T, int size>
bool Pointer<T, size>::first = true;

// Constructor for both initialized and uninitialized objects. -> see class interface
template<class T,int size>
Pointer<T,size>::Pointer(T *t): addr(t) {
    // Register shutdown() as an exit function.
    if (first){
        atexit(shutdown);
        first = false;
    }
    // TODO: Implement Pointer constructor
    // Lab: Smart Pointer Project Lab
    std::unique_lock lock(_mutex_);

    auto ptrDetailsIter = refContainer.find(t);
    if(ptrDetailsIter == refContainer.end()){
      if(isArray){
        refContainer.insert_or_assign(t,PtrDetails(t, arraySize));
      } else {
        refContainer.insert_or_assign(t,PtrDetails(t));
      }
      return;
    }
    
    auto& ptrDetails = ptrDetailsIter->second;
    ptrDetails.refcount++;


}
// Copy constructor.
template< class T, int size>
Pointer<T,size>::Pointer(const Pointer &rhs)
{

    // TODO: Implement Pointer constructor
    // Lab: Smart Pointer Project Lab
    auto rhsAddr = rhs.addr;
    
    (T*)this->addr = rhsAddr;
    (T*)this->isArray = rhs.isArray; 
    (T*)this->arraySize = rhs.arraySize;

    std::unique_lock lock(_mutex_);

    auto ptrDetailsIter = refContainer.find(rhsAddr);

    ptrDetailsIter->refcount++;

}

// Destructor for Pointer.
template <class T, int size>
Pointer<T, size>::~Pointer()
{
    
    // TODO: Implement Pointer destructor
    // Lab: New and Delete Project Lab
    
    std::unique_lock lock(_mutex_);

    auto ptrDetailsIter = refContainer.find(addr);

    auto& ptrDetails = ptrDetailsIter->second;

    if(--(ptrDetails.refcount) == 0){
      if(isArray){
        delete[] (ptrDetails.memPtr);
      } else {
        delete (ptrDetails.memPtr);
      }
      refContainer.erase(ptrDetailsIter);
    }

}

// Collect garbage. Returns true if at least
// one object was freed.
/* template <class T, int size>
bool Pointer<T, size>::collect(){
  no
} */
// Overload assignment of pointer to Pointer.
template <class T, int size>
T *Pointer<T, size>::operator=(T *t){

    // TODO: Implement operator==
    // LAB: Smart Pointer Project Lab
    if(t == addr){
      return t;
    }

    std::unique_lock lock(_mutex_);

    auto ptrDetailsIter = refContainer.find(addr);
    auto& ptrDetails =  ptrDetailsIter->second;
    if(--ptrDetails.refcount == 0){
      delete ptrDetails.memPtr;
      refContainer.erase(ptrDetailsIter);
    };

    ptrDetailsIter = refContainer.find(t);
    if(ptrDetailsIter == refContainer.end()){
      if(isArray){
        refContainer.insert_or_assign(t,PtrDetails(t, arraySize));
      } else {
        refContainer.insert_or_assign(t,PtrDetails(t));
      }
    }

    addr = t;
    
    return t;

}
// Overload assignment of Pointer to Pointer.
template <class T, int size>
Pointer<T, size> &Pointer<T, size>::operator=(Pointer &rv){

    // TODO: Implement operator==
    // LAB: Smart Pointer Project Lab
    return (*operator=(&rv));

}

// A utility function that displays refContainer.
template <class T, int size>
void Pointer<T, size>::showlist(){

    std::cout << "refContainer<" << typeid(T).name() << ", " << size << ">:\n";
    std::cout << "memPtr refcount value\n ";
    if (refContainer.begin() == refContainer.end())
    {
        std::cout << " Container is empty!\n\n ";
    }
    
    for (auto p = refContainer.begin(); p != refContainer.end(); p++)
    {
        const auto& ptrDetails = p->second;
        std::cout << "[" << ptrDetails.memPtr << "]"
             << " " << ptrDetails.refcount << " ";
        if (ptrDetails.memPtr)
            std::cout << " " << *ptrDetails.memPtr;
        else
            std::cout << "---";
        std::cout << std::endl;
    }
    
    std::cout << std::endl;


}
// Find a pointer in refContainer.
/* template <class T, int size>
typename std::list<PtrDetails<T> >::iterator
Pointer<T, size>::findPtrInfo(T *ptr){
    typename std::list<PtrDetails<T> >::iterator p;
    // Find ptr in refContainer.
    for (p = refContainer.begin(); p != refContainer.end(); p++)
        if (p->memPtr == ptr)
            return p;
    return p;
} */
// Clear refContainer when program exits.
template <class T, int size>
void Pointer<T, size>::shutdown(){
    if (refContainerSize() == 0){
      return; // list is empty
    }

    std::unique_lock lock(_mutex_);

    //Dealocate ptr and remove all PtrDetails
    for (
      auto ptrDetailsIter = refContainer.begin();
      ptrDetailsIter != refContainer.end();
      ptrDetailsIter = refContainer.erase(ptrDetailsIter)
    )
    {
      auto& ptrDetails = ptrDetailsIter->second;
      if(ptrDetails.isArray){
        delete[] (ptrDetails.memPtr);
      } else {
        delete (ptrDetails.memPtr);
      }
    }
}