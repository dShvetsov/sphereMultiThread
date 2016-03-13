#include <stdexcept>
#include <cstring>
#include <list>
#include <map>
#include <cstdio>
#include <iostream>
#include <fstream>

enum class AllocErrorType {
    InvalidFree,
    NoMemory,
    InvalidPtr
};


class AllocError: std::runtime_error {
private:
    AllocErrorType type;

public:
    AllocError(AllocErrorType _type, std::string message):
            runtime_error(message),
            type(_type)
    {}

    AllocErrorType getType() const { return type; }
};

class Allocator;

class Pointer {
    unsigned int number;
    Allocator *allocator;
public:
    Pointer(){number = 0; allocator = NULL;}
    Pointer(unsigned int k, Allocator *_allocator);
    void *get() const;
    friend class Allocator;
};

class Allocator {
    struct Header{
        size_t size;
        bool freed;
    };
    struct Ptr {
        void *place;
        Header *h;
        Ptr(Header *_h){
            h = _h;
            place = (void *)((char *)h + sizeof(Header));
        }
        Ptr(){place = NULL; h=NULL;}
        void *get() const { return place;}
    };
    enum class ActiveMemory{
        First,
        Second
    } activememory;

    void *base;
    void *base1;
    void *base2;
    size_t allsize;
    size_t subsize;
    Ptr _alloc(Header *h, size_t N);
    std::map<unsigned int, Ptr> ptrs;
    unsigned int nextint;
    void sealing();
    void* rebase(Allocator::Ptr *, void *);
    std::ofstream fout;
public:
    
    Allocator(void *_base, size_t size) {
        if (size < 2 * sizeof(Header)) { throw AllocErrorType::NoMemory; }
        allsize = size;
        subsize = size / 2;
        activememory = ActiveMemory::First;
        base = base1 = _base;
        base2 = (void *)((char *)_base +subsize);
        Header *h = (Header *)base;
        h->size = subsize - sizeof(Header);
        h->freed = true;
        nextint = 1;
    }
    Pointer alloc(size_t N);
    void realloc(Pointer &p, size_t N);
    void free(Pointer &p);

    void defrag();
    std::string dump() { return ""; }
    friend class Pointer;
    ~Allocator() {fout.close();}
};

