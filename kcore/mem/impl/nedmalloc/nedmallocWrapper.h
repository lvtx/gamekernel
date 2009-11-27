#pragma once

#include "nedmalloc_v104_svn1040/nedmalloc.h"

namespace gk
{
/**
 * @class   nedmallocWrapper
 *
 * @brief   Allocator implementation that makes use of nedmalloc
 *          (http://www.nedprod.com/programs/portable/nedmalloc/).
 */
class nedmallocWrapper
{
public:
    ~nedmallocWrapper();

    void* Alloc(size_t size);
    void Free(void* p);
};

inline
nedmallocWrapper::~nedmallocWrapper()
{
    nedalloc::nedmalloc_trim(0);
}

inline
void*
nedmallocWrapper::Alloc(size_t size)
{
    return nedalloc::nedmalloc(size);
}

inline
void
nedmallocWrapper::Free(void* p)
{
    nedalloc::nedfree(p);
}

} // gk
