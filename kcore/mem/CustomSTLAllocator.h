///////////////////////////////////////////////////////////////////////////////
//
//  CustomSTLAllocator.h
//
//  Heap-based allocator. Uses a unique heap per allocator per type T. Based
//  on Win32 HeapCreate and Allocator. Could be easily customized to not
//  serialize heap access for single-threaded programs (see HeapCreate
//  HEAP_NO_SERIALIZE option).
//
//  Copyright ?2002 Pete Isensee (PKIsensee@msn.com).
//  All rights reserved worldwide.
//
//  Permission to copy, modify, reproduce or redistribute this source code is
//  granted provided the above copyright notice is retained in the resulting 
//  source code.
// 
//  This software is provided "as is" and without any express or implied
//  warranties.
//
///////////////////////////////////////////////////////////////////////////////

// The original version is from Game Programming Gems III 
//
// Here we modified for our MemoryManager for multithreaded environment
// 
// 

#pragma once

#include <kcore/mem/Allocator.h>

#ifdef max 
#undef max   // to protect from name pollution with a macro 
#endif 

#include <memory>
#include <limits>

namespace gk 
{

/**
 * @class CustomSTLAllocator 
 *
 * @brief Allocates STL memory from Default Allocator
 */
template <typename T>
class CustomSTLAllocator
{
public:
    // Typedefs
    typedef size_t    size_type;
    typedef ptrdiff_t difference_type;
    typedef T*        pointer;
    typedef const T*  const_pointer;
    typedef T&        reference;
    typedef const T&  const_reference;
    typedef T         value_type;

    // Constructors
    CustomSTLAllocator() throw() {}

    CustomSTLAllocator( const CustomSTLAllocator& ) throw() {}

#if _MSC_VER >= 1300 // VC6 can't handle template members
    template <typename U>
    CustomSTLAllocator( const CustomSTLAllocator<U>& ) throw() {}
#endif

    CustomSTLAllocator& operator=( const CustomSTLAllocator& )
    {
        // nothing to do
        return *this;
    }

    // Destructor
    ~CustomSTLAllocator() throw()
    {
    }

    // Utility functions
    pointer address( reference r ) const
    {
        return &r;
    }

    const_pointer address( const_reference c ) const
    {
        return &c;
    }

    size_type max_size() const
    {
        NS_ASSERT( sizeof( T ) > 0 );

        return std::numeric_limits<size_t>::max() / sizeof(T);
    }

    // In-place construction
    void construct( pointer p, const_reference c )
    {
        // placement new operator
        new( reinterpret_cast<void*>(p) ) T(c);
    }

    // In-place destruction
    void destroy( pointer p )
    {
        p;
        // call destructor directly
        (p)->~T();  // XXX : This causes unreferenced formal parameter ( C4100 ) warning. strange.
    }

    // Rebind to allocators of other types
    template <typename U>
    struct rebind
    {
        typedef CustomSTLAllocator<U> other;
    };

    // Allocate raw memory
    pointer allocate( size_type n, const void* = NULL )
    {
        size_type bytes = n * sizeof(T);

        void* p = g_allocator.Alloc( bytes );

        if( p == 0 )
            throw std::bad_alloc();

        return pointer(p);
    }

    // Free raw memory.
    // Note that C++ standard defines this function as
    // deallocate( pointer p, size_type). Because Visual C++ 6.0
    // compiler doesn't support template rebind, Dinkumware uses
    // void* hack.
    void deallocate( void* p, size_type )
    {
        // The standard states that p must not be NULL. However, some
        // STL implementations fail this requirement, so the check must
        // be made here.
        NS_RETURN_IF ( p == 0 );

        g_allocator.Free( p );
    }

    // Non-standard Dinkumware hack for Visual C++ 6.0 compiler.
    // VC 6 doesn't support template rebind.
    char* _Charalloc( size_type nBytes )
    {
        char* p = reinterpret_cast<char*>( g_allocator.Alloc( nBytes ) );

        if( p == 0 )
            throw std::bad_alloc();

        return p;
    }
}; // end of CustomSTLAllocator

// Comparison
template <typename T1>
bool operator==( const CustomSTLAllocator<T1>& lhs, const CustomSTLAllocator<T1>& rhs) throw()
{
    return true; // always true since we are using one MemoryManager instance
}

template <typename T1>
bool operator!=( const CustomSTLAllocator<T1>& lhs, const CustomSTLAllocator<T1>& rhs) throw()
{
    return false; // always false since we are using one MemoryManager instance
}

} // gk
