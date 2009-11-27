#pragma once

#include <kcore/mem/Allocator.h>

namespace gk 
{
/**
 * @class   AllocatorAware
 *
 * @brief   Superclass for those whose instances are to be allocated/deallocated 
 *          by DefaultAllocator.
 * 
 * In order for the class-wide delete operator to be called properly, the 
 * target pointer should be validly typed. If you try to delete a pointer 
 * casted into (void*) type, the global delete operator will be called instead 
 * of the class-wide delete operator.
 */
class AllocatorAware
{
public:
    /** 
	 * Virtual dtor for sub-classing. 
	 */
    virtual ~AllocatorAware();

    /** 
	 * Class-wide new operator which relies on DefaultAllocator. 
	 */
    static void* operator new(size_t size);

    /** 
	 * Class-wide delete operator which relies on DefaultAllocator.
	 */
    static void operator delete(void* p);
};

inline
AllocatorAware::~AllocatorAware()
{
}

inline
void*
AllocatorAware::operator new(size_t size)
{
    return g_allocator.Alloc(size);
}

inline
void
AllocatorAware::operator delete(void* p)
{
    g_allocator.Free(p);
}

} // gk 
