#include "allocator.h"

Pointer::Pointer(unsigned int k, Allocator *_allocator){
        allocator = _allocator;
        number = k;
    }
void *Pointer::get() const { return ((allocator->ptrs)[number]).get(); } 

Allocator::Ptr Allocator::_alloc(Header *h, size_t N){
	if (h->size < N) {throw AllocErrorType::NoMemory;}
	size_t last_size = h->size;
	int tmp = last_size - (N + sizeof(Header));
	if (tmp > 0){
		Header *g = (Header *)((char *)h + N + sizeof(Header));
		g->size = tmp;
		h->size = N;
		g->freed = true;
	}
	h->freed = false;
	return Ptr(h);
}

void Allocator::sealing() {
	char *cur = (char *)base;
	Header *h2, *h1;
	h1 = (Header *)cur;
	cur += h1->size + sizeof(Header);
	h2 = (Header *)cur; 
	while (cur - (char *)base <= subsize){
		if (h1->freed && h2->freed){
			h1->size += (h2->size + sizeof(Header));
		} else {
			h1 = h2;
		}
		cur += h2->size + sizeof(Header);
		h2 = (Header *)cur;
	}
}

void Allocator::realloc(Pointer &p, size_t N){
	Ptr *ptr;
	auto it = ptrs.find(p.number);
	if (it == ptrs.end()){
		p = alloc(N);
		return;
	}
/*	try {
		ptr = &(ptrs.at(p.number));
	} catch(std::out_of_range &){
		throw AllocError(AllocErrorType::InvalidPtr, 
			"try realloc nonexisting piece of memory");
	} */
	ptr = &it->second;
	Header *h = ptr->h;
	char *cur = (char *)h;
	size_t oldsize = h->size;
	cur += h->size + sizeof(Header);
	Header *h2 = (Header *)cur;
	if (h2->freed && h2->size + h->size + sizeof(Header) >= N){
		size_t tmp = h2->size + h->size + sizeof(Header);
		if (tmp - N > 2 * sizeof(Header)){
			h->size = N;
			cur = (char *)h + N + sizeof(Header);
			h2 = (Header *)cur;
			h2->freed = true;
			h2->size = tmp - (N + sizeof(Header));
			return;
		} else
		{
			h->size += h2->size + sizeof(Header);
			return;
		}
	}
	Pointer tmpp = alloc(N);
	std::memcpy(tmpp.get(), p.get(), oldsize);
	Allocator::free(p);
	p = tmpp;
}

void Allocator::free(Pointer &p) {
	unsigned int n = p.number;
	Ptr ptr;
	try{
	  ptr = ptrs.at(n);
	} catch(std::out_of_range &t){
		throw AllocError(AllocErrorType::InvalidFree, "free unallocated memory");
	}
	auto h = ptr.h;
	h->freed = true;
	ptrs.erase(n);
	sealing();
}

void Allocator::defrag(){
	void *old, *nevv;
	if (activememory == ActiveMemory::First){
		old = base1;
		nevv = base = base2;
		activememory = ActiveMemory::Second;
	} else {
		old = base2;
		nevv = base = base1;
		activememory = ActiveMemory::First;
	}
	char *cur = (char *)base;
	for (auto i = ptrs.begin(); i != ptrs.end(); i++){
			cur = (char *)rebase(&(i->second), (void *)cur);
	}
	Header *h = (Header *)cur;
	h->freed = true;
	h->size = subsize - (cur - (char*)base) - sizeof(Header);
/*	Header *freeside = (Header *)old;
	freeside->freed = true;
	freeside->size = subsize - sizeof(Header); */
}

void *Allocator::rebase(Allocator::Ptr *ptr, void *newplace){
	Header *h = ptr->h;
	size_t size = h->size;
	std::memcpy(newplace, h, size + sizeof(Header));
	ptr->h = (Header *)newplace;
	ptr->place = (void *)((char *) newplace + sizeof(Header));
	return (void *)((char *)newplace + size + sizeof(Header));
}

Pointer Allocator::alloc(size_t N)
{

	char *cur = (char *)base;
	Ptr p;
	Header *h;
	for (;;){
		h = (Header *)cur;
		if (h->freed && h->size >= N){
			p = _alloc(h, N);
			break;
		}
		cur += h->size + sizeof(Header);
		if (cur - (char*)base > subsize){
			throw AllocError(AllocErrorType::NoMemory,
			  std::string("dont enough memory, try defrug"));
		}
	}
	ptrs[nextint] = p;
	return Pointer(nextint++, this);
	
}
