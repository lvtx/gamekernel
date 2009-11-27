#pragma once

#include <malloc.h>
#include <stdlib.h>

#include <new>

namespace gk
{
/**
 * @class   MSVCRT_malloc
 *
 * @brief   Allocator implementation that directly makes use of Microsoft
 *          VisualC/C++ CRT (C Runtime Library) malloc/free.
 */
class MSVCRT_malloc
{
public:
	void* Alloc(size_t size);
	void Free(void* p);
};

inline
void*
MSVCRT_malloc::Alloc(size_t size)
{
    void* p = malloc(size);
    if (!p) {
        throw std::bad_alloc(); // ANSI/ISO compliant behavior
    }
    return p;
}

inline
void
MSVCRT_malloc::Free(void* p)
{
    free(p);
}

} // gk
