#include "stdafx.h"

#include <kcore/mem/Allocator.h>

#include <memory.h>

namespace gk 
{

DefaultAllocator g_allocator; // global default allocator

} // gk

#ifdef OVERRIDE_GLOBAL_NEW_DELETE

// Global namespace

/** Global new operator which relies on DefaultAllocator. */
inline
void*
operator new(size_t size)
{
    return gk::g_allocator.Alloc(size); 
}

/** Global new[] operator which relies on DefaultAllocator. */
inline
void*
operator new[](size_t size)
{
    return gk::g_allocator.Alloc(size); 
}

/** Global delete operator which relies on DefaultAllocator. */
inline
void
operator delete(void* p)
{
    gk::g_allocator.Free(p);
}

/** Global delete[] operator which relies on DefaultAllocator. */
inline
void
operator delete[](void* p)
{
    gk::g_allocator.Free(p);
}

#endif // OVERRIDE_GLOBAL_NEW_DELETE

// EOF Allocator.cpp
