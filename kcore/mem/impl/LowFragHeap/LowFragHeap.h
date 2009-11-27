#pragma once

#include <new>

namespace gk
{
/**
 * @class   LowFragHeap
 *
 * @brief   Allocator implementation that makes use of Low-fragmentation Heap
 *          available on Windows XP & 2003 Server.
 */
class LowFragHeap
{
public:
    LowFragHeap();

    void* Alloc(size_t size);
    void Free(void* p);

private:
    HANDLE procHeap_;
};

inline
LowFragHeap::LowFragHeap()
{
    ULONG HeapFragValue = 2;

    procHeap_ = GetProcessHeap();

    HeapSetInformation( procHeap_
                      , HeapCompatibilityInformation
                      , &HeapFragValue
                      , sizeof(HeapFragValue)
                      );
    HeapQueryInformation( procHeap_
                        , HeapCompatibilityInformation
                        , &HeapFragValue
                        , sizeof(HeapFragValue)
                        , NULL
                        );
    if (HeapFragValue != 2) {
        // Windows Low-fragment Heap won't work under a debugger!
    }
}

inline
void*
LowFragHeap::Alloc(size_t size)
{
    void* p = HeapAlloc(procHeap_, 0, size);
    if (!p) {
        throw std::bad_alloc(); // ANSI/ISO compliant behavior
    }
    return p;
}

inline
void
LowFragHeap::Free(void* p)
{
    if (!p) { return; } // may not pass a NULL pointer to HeapFree
    HeapFree(procHeap_, 0, p);
}

} // gk
