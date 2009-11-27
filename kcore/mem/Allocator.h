#pragma once

//#define ALLOC_IMPL          MSVCRT_malloc
//#define ALLOC_IMPL_INCLUDE  <kcore/mem/impl/MSVCRT_malloc/MSVCRT_malloc.h>

//#define ALLOC_IMPL          LowFragHeap
//#define ALLOC_IMPL_INCLUDE  <kcore/mem/impl/LowFragHeap/LowFragHeap.h>

#define ALLOC_IMPL          nedmallocWrapper
#define ALLOC_IMPL_INCLUDE  <kcore/mem/impl/nedmalloc/nedmallocWrapper.h>

#include ALLOC_IMPL_INCLUDE

// Override global new/delete operators only in release builds.
#ifndef _DEBUG
 #define OVERRIDE_GLOBAL_NEW_DELETE
 #define REPLACE_SYSTEM_ALLOCATOR			// for lua to use nedmalloc
#endif

namespace gk 
{
/**
 * @class   Allocator
 *
 * @brief   Template class wrapping a specific allocator implementation.
 */
template<class AllocImpl>
class Allocator
{
public:
    /** 
	 * Default ctor. 
	 */
    Allocator();

    /** 
	 * Default dtor. 
	 */
    ~Allocator();

    /** 
	 * Allocates a new memory block of the given size. 
	 */
    void* Alloc(size_t size);

    /** 
	 * Deallocates the memory block pointed by the given pointer. 
	 */
    void Free(void* p);

    /** 
	 * Compilers may pass the object size to the class-wide operator delete, 
     * and some allocator implementations might rely on this feature.
     * Note that this is never used in case of overriding global new/delete.
     */
    void Free(void* p, size_t size);

private:
    AllocImpl impl_;
    
    // Copy protection
    Allocator(const Allocator&);
    Allocator& operator=(const Allocator&);
};

template<class C>
inline
Allocator<C>::Allocator()
{
}

template<class C>
inline
Allocator<C>::~Allocator()
{
}

template<class C>
inline
void*
Allocator<C>::Alloc(size_t size)
{
    void* p = impl_.Alloc(size);
    return p;
}

template<class C>
inline
void
Allocator<C>::Free(void* p)
{
    impl_.Free(p);
}

template<class C>
inline
void
Allocator<C>::Free(void* p, size_t size)
{
    impl_.Free(p, size);
}

typedef Allocator<ALLOC_IMPL> DefaultAllocator; ///< By redefining ALLOC_IMPL, default allocator can be changed. 

extern DefaultAllocator g_allocator;

} // namespace gk 

